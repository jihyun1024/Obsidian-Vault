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
#include <openssl/rand.h>
#include <openssl/err.h>
#include "Practical.h"
#include "OTPlib.h"

#include <time.h>
#include <math.h>

static const int MAXPENDING = 5; // Maximum outstanding connection requests

// 간단한 사용자 데이터베이스 정의
#define MAX_USER_NUMBER         200                             // 최대 수용 가능한 사용자의 수
#define MAX_USER_ACCOUNT_LEN    100                              // 사용자의 아이디와 비밀번호 최대 자리수

char    list_user_id[MAX_USER_NUMBER][MAX_USER_ACCOUNT_LEN];    // 현재 등록된 사용자의 아이디 목록
char    list_user_pwd[MAX_USER_NUMBER][MAX_USER_ACCOUNT_LEN];   // 현재 등록된 사용자의 비밀번호 목록
int     registered_user_num;                                    // 현재 등록된 사용자 수
//-------------------------------------------------------------------------

// 새로운 사용자 등록 (즉 사용자의 아이디와 비밀번호를 사용자 데이터베이스에 추가)
int add_user_2_db(const char *id, const char* pwd)
{
    if(id == NULL || pwd == NULL)
        DieWithUserMessage("add_user_2_db() failed", "invalid input id, pwd");

    if (registered_user_num >= MAX_USER_NUMBER)
        DieWithUserMessage("add_user_2_db() failed", "user database is full");

    int len;
   
    len = strlen(id);
    if(len > MAX_USER_ACCOUNT_LEN-1)
        DieWithUserMessage("add_user_2_db() failed", "invalid input id, pwd");
    strncpy(list_user_id[registered_user_num], id, len);
    list_user_id[registered_user_num][len] = '\0';

    len = strlen(pwd);
    if (len > MAX_USER_ACCOUNT_LEN - 1)
        DieWithUserMessage("add_user_2_db() failed", "invalid input id, pwd");
    strncpy(list_user_pwd[registered_user_num], pwd, len);
    list_user_pwd[registered_user_num][len] = '\0';

    registered_user_num++;

    return registered_user_num;
}

// 사용자 데이터베이스 초기화 (3명의 사용자 등록함)
void init_user_db()
{
    // initialization
    memset(list_user_id, 0, sizeof(list_user_id));
    memset(list_user_pwd, 0, sizeof(list_user_pwd));
    registered_user_num = 0;

    add_user_2_db("CryptoWorld", "1234");
    add_user_2_db("kmuISCM", "1233");
    add_user_2_db("dr.You", "1901");
    // 이곳에 더 추가하고자 하는 사용자를 추가하시기 바랍니다.

    /* 사용자 추가후 확인을 하기 위한 코드 - 주석처리
    for (int i = 0; i < MAX_USER_NUMBER; ++i) 
    {
        printf("id[%d] = '%s', pwd[%d] = '%s'\n",
            i, list_user_id[i], i, list_user_pwd[i]);
    }
    printf("registered_user_num = %d\n", registered_user_num);
    */
}

static void hexprint(const unsigned char* p, size_t len) {
    for (size_t i = 0; i < len; ++i) printf("%02x", p[i]);
    printf("\n");
}

/* 상수시간(비교시간이 입력 길이에 민감하게 변하지 않도록) 문자열 동등 비교.
   - `fixed`는 내부 저장 버퍼 (길이: fixed_len)
   - `var`는 외부 입력
   - fixed_len은 보통 MAX_USER_ACCOUNT_LEN 로 전달
   반환: 같으면 1, 다르면 0
*/
static bool const_time_str_eq_fixed(const char* fixed, const char* var, size_t var_len, size_t fixed_len)
{
    unsigned char diff = 0;
    for (size_t i = 0; i < fixed_len; ++i) {
        unsigned char a = (unsigned char)fixed[i];                      /* 저장된 바이트 (버퍼 범위 내) */
        unsigned char b = (unsigned char)(i < var_len ? var[i] : '\0'); /* 입력 바이트 또는 '\0'로 패딩 */
        diff |= (a ^ b); /* 배타적 논리합으로 같으면 0 다르면 1이 나오도록 함 */
        if (diff != 0) break;
    }
    /* 입력이 fixed_len 이상이면 길이 불일치로 처리(상수 시간에 가깝게 반영) */
    diff |= (unsigned char)(var_len >= fixed_len ? 1 : 0);

    return (diff == 0);
}

char* is_user_there(const char* id, int id_len)
{
    if (id == NULL || id_len < 0 || id_len > MAX_USER_ACCOUNT_LEN)
        return NULL;

    int n = registered_user_num;
    if (n < 0) n = 0;
    if (n > MAX_USER_NUMBER) n = MAX_USER_NUMBER;

    for (int i = 0; i < MAX_USER_NUMBER; ++i){
        bool id_eq = const_time_str_eq_fixed(list_user_id[i], id, (size_t)id_len, MAX_USER_ACCOUNT_LEN);
        if (id_eq) return list_user_pwd[i];
    }

    return NULL;
}

char* recv_id(int c_sock, char* p_id)
{
    if (c_sock < 0 || p_id == NULL)
        DieWithUserMessage("recv_msg() failed", "invalid socket descryptor or id");

    char id[MAX_USER_ACCOUNT_LEN];
    int numBytes;
    int len = 0;

    memset(id, 0, MAX_USER_ACCOUNT_LEN);
    numBytes = recv(c_sock, id, MAX_USER_ACCOUNT_LEN -1, 0);
    if (numBytes < 0)
        DieWithSystemMessage("recv() failed");
    else if (numBytes < 1)
        return NULL;

    len = strlen(id);
    strncpy(p_id, id, len);
    return is_user_there(id, len);
}


void recv_response(int c_sock, char* p_resp, int *p_resp_len)
{
    if (c_sock < 0 || p_resp == NULL || p_resp_len == NULL)
        DieWithUserMessage("recv_response() failed", "invalid input parameters");

    int numBytes;
   
    memset(p_resp, 0, *p_resp_len);
    numBytes = recv(c_sock, p_resp, *p_resp_len, 0);
    if(numBytes < 0)
        DieWithSystemMessage("recv() failed");

    *p_resp_len = numBytes;
}

int main(int argc, char *argv[]) {
  char buffer[BUFSIZE];
  ssize_t len = 0, numBytesSent = 0;
          
  if (argc != 2) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)", "<Server Port>");

  in_port_t servPort = atoi(argv[1]); // First arg:  local port
  // user database initialization
  init_user_db();

  // Create socket for incoming connections
  int servSock; // Socket descriptor for server
  if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithSystemMessage("socket() failed");

  // Construct local address structure
  struct sockaddr_in servAddr;                  // Local address
  memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
  servAddr.sin_family = AF_INET;                // IPv4 address family
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
  servAddr.sin_port = htons(servPort);          // Local port

  // Bind to the local address
  if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
    DieWithSystemMessage("bind() failed");

  // Mark the socket so it will listen for incoming connections
  if (listen(servSock, MAXPENDING) < 0)
    DieWithSystemMessage("listen() failed");

  for (;;) {
      struct sockaddr_in clntAddr; // Client address
      socklen_t clntAddrLen = sizeof(clntAddr);
      char id[MAX_USER_ACCOUNT_LEN], resp[OTP_RESP_SIZE];
      int resp_len;

      int clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &clntAddrLen);
      if (clntSock < 0)
          DieWithSystemMessage("accept() failed");

      fputs("\n>>>> Connected to Client: ", stdout);
      PrintSocketAddress((struct sockaddr*)&clntAddr, stdout);
      
      memset(id, 0, MAX_USER_ACCOUNT_LEN);
      char* pwd = recv_id(clntSock, id);
      if (pwd == NULL){
            fputs("\nThe user does not exist in database\n", stdout);
            close(clntSock);
            continue; }
      printf("\nThe user %s starts its authentication procedure: ", id);

      /* 1. CHALLENGE값 생성 */
      unsigned char chal[OTP_CHAL_SIZE];
      if (1 != RAND_bytes(chal, OTP_CHAL_SIZE))
          DieWithSystemMessage("RAND_bytes(chal) failed");
      printf("Plain Challenge (size %d): ", OTP_CHAL_SIZE);
      hexprint(chal, OTP_CHAL_SIZE);

      /* 2. KEY/IV 생성 및 암호화 */
      unsigned char key[AES128_KEY_LEN];
      derive_key(pwd, key);

      unsigned char iv[AES_BLOCK_LEN];
      if (1 != RAND_bytes(iv, sizeof(iv)))
          DieWithSystemMessage("RAND_bytes(iv) failed");

      unsigned char ct[OTP_CHAL_SIZE + AES_BLOCK_LEN]; /* 16B + padding → 최대 32B */
      int ct_len = aes_encrypt(chal, OTP_CHAL_SIZE, key, iv, ct);
      if (ct_len <= 0) DieWithUserMessage("aes_encrypt()", "encryption failed");

      /* 3. IV + CT 전송 */
      unsigned char sendbuf[AES_BLOCK_LEN + 64];
      memcpy(sendbuf, iv, AES_BLOCK_LEN);
      memcpy(sendbuf + AES_BLOCK_LEN, ct, ct_len);
      int total_send_len = AES_BLOCK_LEN + ct_len;

      printf("IV: "); hexprint(iv, AES_BLOCK_LEN);
      printf("CT: "); hexprint(ct, ct_len);

      numBytesSent = send(clntSock, sendbuf, total_send_len, 0);
      if (numBytesSent < 0)
          DieWithSystemMessage("send() failed");
      else if (numBytesSent != total_send_len)
          DieWithUserMessage("send()", "sent unexpected number of bytes");

      /* 4. 클라이언트 응답 수신 */
      memset(resp, 0, OTP_RESP_SIZE);
      resp_len = OTP_RESP_SIZE;
      recv_response(clntSock, resp, &resp_len);
      printf("Client RESP(%dB): ", resp_len);
      hexprint((const unsigned char*)resp, resp_len);

      /* 5. verify_otp()로 검증 */
      bool ok = verify_otp(pwd, (char*) chal, resp, resp_len);
      if (ok) {
        sprintf(buffer, "Welcome %s", id);
        printf("The user %s is successfully authenticated\n", id);
      } else {
        sprintf(buffer, "Authentication failed for %s", id);
        printf("The user %s failed authentication\n", id);
        }
      len = strlen(buffer);

      numBytesSent = send(clntSock, buffer, len, 0);
      if (numBytesSent < 0)
          DieWithSystemMessage("send() failed");
      else if (numBytesSent != len)
          DieWithUserMessage("send()", "sent unexpected number of bytes");

      close(clntSock);
  }
  close(servSock);
}
