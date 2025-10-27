#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <openssl/opensslv.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/err.h>

#include <time.h>
#include <math.h>
#include "Practical.h"
#include "OTPlib.h"

#define ENC_CHAL_TOTAL  (AES_BLOCK_LEN + 32)

static void hexprint(const unsigned char* p, size_t len) {
    for (size_t i = 0; i < len; ++i) printf("%02x", p[i]);
    printf("\n");
}

// IV + CT
void get_challenge(int sock, const char* id, char* out_chall)
{
    if(sock < 0 || id == NULL || out_chall == NULL)
        DieWithUserMessage("get_challenge() failed", "invalid args");

    int len = strlen(id);
    int numBytes = send(sock, id, len, 0);
    if (numBytes < 0) DieWithSystemMessage("send() failed");
    else if (numBytes != len) DieWithUserMessage("send()", "sent unexpected number of bytes");

    unsigned char encbuf[ENC_CHAL_TOTAL];
    memset(encbuf, 0, sizeof(encbuf));
    int recvd = 0;
    while (recvd < (int)sizeof(encbuf)) {
        int r = recv(sock, encbuf + recvd, (int)sizeof(encbuf) - recvd, 0);
        if (r < 0) DieWithSystemMessage("recv() failed");
        if (r == 0) break;
        recvd += r;
    }
    if (recvd != (int)sizeof(encbuf))
        DieWithUserMessage("get_challenge()", "unexpected encrypted challenge length");

    unsigned char iv[AES_BLOCK_LEN];
    memcpy(iv, encbuf, AES_BLOCK_LEN);
    unsigned char ct[32];
    memcpy(ct, encbuf + AES_BLOCK_LEN, 32);

    printf("Received IV: "); hexprint(iv, sizeof(iv));
    printf("Received CT: "); hexprint(ct, sizeof(ct));
    memcpy(out_chall, encbuf, sizeof(encbuf));
}

void send_response(int sock, char* resp){
    if (sock < 0 || resp == NULL)
        DieWithUserMessage("send_response() failed", "invalid input parameters - socket descryptor, response");

    int numBytes = send(sock, resp, OTP_RESP_SIZE, 0);
    if (numBytes < 0)
        DieWithSystemMessage("send() failed");
    else if (numBytes != OTP_RESP_SIZE)
        DieWithUserMessage("send()", "sent unexpected number of bytes");

    fputs("Authentication Result: ", stdout);
    do
    {
        char buffer[BUFSIZE];

        numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
        if (numBytes < 0)
            DieWithSystemMessage("recv() failed");
        else if (numBytes == 0)
            break;

        buffer[numBytes] = '\0';
        fputs(buffer, stdout);
    } while (1);
}

int main(int argc, char *argv[]) {

  if (argc < 5 || argc > 6) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "<Server Address> <User ID> <User Password> [<Server Port>]");

  char buffer[BUFSIZE];
  char *servIP  = argv[1];     // First arg: server IP address (dotted quad)
  char* userID   = argv[2];     // First arg: server IP address (dotted quad)
  char* userPWD  = argv[3];     // First arg: server IP address (dotted quad)

  // Third arg (optional): server port (numeric).  7 is well-known echo port
  in_port_t servPort = (argc == 5) ? atoi(argv[4]) : 7;

  // Create a reliable, stream socket using TCP
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    DieWithSystemMessage("socket() failed");

  // Construct the server address structure
  struct sockaddr_in servAddr;            // Server address
  memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
  servAddr.sin_family = AF_INET;          // IPv4 address family
  // Convert address
  int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
  if (rtnVal == 0)
    DieWithUserMessage("inet_pton() failed", "invalid address string");
  else if (rtnVal < 0)
    DieWithSystemMessage("inet_pton() failed");
  servAddr.sin_port = htons(servPort);    // Server port

  // Establish the connection to the echo server
  if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithSystemMessage("connect() failed");

  fputs("Connected to Server: ", stdout);
  PrintSocketAddress((struct sockaddr*)&servAddr, stdout);
  sprintf(buffer, "\nAuthentication Request to Server: %s\n", userID);
  fputs(buffer, stdout);
  fflush(stdout);

  /* 1. IV + CT 챌린지값 수신 */
  unsigned char enc_chal[ENC_CHAL_TOTAL];
  memset(enc_chal, 0, sizeof(enc_chal));
  get_challenge(sock, userID, (char*)enc_chal);

  /* 2. 복호화하여 CHALLENGE값 복원 */
  unsigned char iv[AES_BLOCK_LEN];
  unsigned char ct[32];
  memcpy(iv, enc_chal, AES_BLOCK_LEN);
  memcpy(ct, enc_chal + AES_BLOCK_LEN, 32);

  unsigned char key[AES128_KEY_LEN];
  derive_key(userPWD, key);

  unsigned char chal[OTP_CHAL_SIZE + AES_BLOCK_LEN];
  int plain_len = aes_decrypt(ct, 32, key, iv, chal);
  if (plain_len != OTP_CHAL_SIZE)
      DieWithUserMessage("Authentication fialed", "ID or Key is invalid");
  printf("\nDecrypted Challenge (size: %d): ", plain_len);
  hexprint(chal, plain_len);

  /* 3. calc_otp() 결과 전송 */
  char resp[OTP_RESP_SIZE];
  int resp_len = 0;
  if (!calc_otp(userPWD, (char*) chal, (char*)resp, &resp_len)) {
    DieWithUserMessage("calc_otp()", "failed to compute response");
    }
  if (resp_len != OTP_RESP_SIZE) {
    DieWithUserMessage("calc_otp()", "unexpected response length");
    }
  printf("\nResponse: ");
  hexprint((const unsigned char*) resp, OTP_RESP_SIZE);
  send_response(sock, (char*)resp);

  close(sock);
}
