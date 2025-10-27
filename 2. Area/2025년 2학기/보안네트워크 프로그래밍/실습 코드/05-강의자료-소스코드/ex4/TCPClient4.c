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

static void hexprint(const unsigned char* p, size_t len) {
    for (size_t i = 0; i < len; ++i) printf("%02x", p[i]);
    printf("\n");
}

void send_msg(int sock, char* id, char* pwd)
{
    if(sock < 0 || id == NULL || pwd == NULL) 
        DieWithUserMessage("send_msg() failed", "invalid input parameters - socket descryptor, id, otp");
    
    char buffer[BUFSIZE], otp[BUFSIZE];
    int len, id_len;
    int numBytes;
    
    memset(buffer, 0, BUFSIZE);
    memset(otp, 0, BUFSIZE);

    if(!calc_otp(pwd, otp, &numBytes))
        DieWithUserMessage("send_msg() failed", "calc_otp() error");
    printf("\nOTP: ");
    hexprint((const unsigned char*) otp, numBytes);
    id_len = strlen(id);
    len = id_len + 1 + numBytes;
    if(len > BUFSIZE)
        DieWithUserMessage("send_msg() failed", "buffer size is small");
    
    for (int i = 0; i < id_len; i++) buffer[i] = id[i];
    buffer[id_len] = ';';
    for (int i = 0; i < numBytes; i++) buffer[id_len + i + 1] = otp[i];

    numBytes = send(sock, buffer, len, 0);
    if (numBytes < 0)
        DieWithSystemMessage("send() failed");
    else if (numBytes != len)
        DieWithUserMessage("send()", "sent unexpected number of bytes");
}

int main(int argc, char *argv[]) {

  if (argc < 5 || argc > 6) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "<Server Address> <User ID> <User Password> [<Server Port>]");

  char *servIP  = argv[1];     // First arg: server IP address (dotted quad)
  char* useID   = argv[2];     // First arg: server IP address (dotted quad)
  char* usePWD  = argv[3];     // First arg: server IP address (dotted quad)

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

  printf("Connected to Server: ");
  PrintSocketAddress((struct sockaddr*)&servAddr, stdout);

  printf("\nSending User Account to Server: %s/%s", useID, usePWD);
  send_msg(sock, useID, usePWD);

  unsigned int numBytes = 0;
  fputs("Received: ", stdout);
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

  close(sock);
}
