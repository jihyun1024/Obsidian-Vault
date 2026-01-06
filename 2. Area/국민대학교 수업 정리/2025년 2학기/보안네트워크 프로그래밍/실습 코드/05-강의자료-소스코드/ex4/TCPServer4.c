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

bool authenticate_user(const char* id, int id_len, const char* pwd, int pwd_len)
{
    if (id == NULL || pwd == NULL || id_len < 0 || id_len > MAX_USER_ACCOUNT_LEN 
        || pwd_len < 0 || pwd_len > MAX_USER_ACCOUNT_LEN)
        return false;

    int n = registered_user_num;
    if (n < 0) n = 0;
    if (n > MAX_USER_NUMBER) n = MAX_USER_NUMBER; /* 안전하게 클램프 */

    for (int i = 0; i < MAX_USER_NUMBER; ++i) 
    {
        bool id_eq = const_time_str_eq_fixed(list_user_id[i], id, (size_t) id_len, MAX_USER_ACCOUNT_LEN);
        bool pwd_eq = verify_otp((char*) list_user_pwd[i], (char*) pwd, pwd_len);
        if (id_eq & pwd_eq) return true;
     }

    return false;
}

void recv_msg(int c_sock, char* p_id, int *p_id_len, char* p_pwd, int *p_pwd_len)
{
    if (c_sock < 0 || p_id == NULL || p_pwd == NULL || p_id_len == NULL || p_pwd_len == NULL)
        DieWithUserMessage("recv_msg() failed", "invalid input parameters");

    char buffer[BUFSIZE];
    int numBytes;
    int i = 0;
   
    memset(buffer, 0, BUFSIZE);
    numBytes = recv(c_sock, buffer, BUFSIZE, 0);
    if(numBytes < 0)
        DieWithSystemMessage("recv() failed");
    else if(numBytes == 0)
        return;

    while (buffer[i] != ';' && i < numBytes){
        p_id[i] = buffer[i]; i++;
    }
    if (i >= numBytes)
        DieWithUserMessage("recv_msg() failed", "there is no semicolon in the received message from client");
    
    *p_id_len = i++;
//    i++;
    *p_pwd_len = numBytes - i;
    while (i < numBytes) {
        p_pwd[i - *p_id_len - 1] = buffer[i];
        i++;
    }
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
      char id[MAX_USER_ACCOUNT_LEN], pwd[MAX_USER_ACCOUNT_LEN];
      int id_len, pwd_len;

      int clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &clntAddrLen);
      if (clntSock < 0)
          DieWithSystemMessage("accept() failed");

      printf("\n>>>> Connected to Client: ");
      PrintSocketAddress((struct sockaddr*)&clntAddr, stdout);
      
      memset(id, 0, MAX_USER_ACCOUNT_LEN);
      memset(pwd, 0, MAX_USER_ACCOUNT_LEN);
      recv_msg(clntSock, id, &id_len, pwd, &pwd_len);
      printf("\n%s's OTP: ", id);
      hexprint((const unsigned char*) pwd, pwd_len);
      if(authenticate_user(id, id_len, pwd, pwd_len)){
          sprintf(buffer, "Welcome %s", id);
          printf("The user %s is successfully authenticated\n", id);
          len = strlen(buffer);
      }
      else{
          strcpy(buffer, "Authentication failed. Please re-enter your username and password");
          printf("The user %s is declined\n", id);
          len = strlen(buffer);
      }

      numBytesSent = send(clntSock, buffer, len, 0);
      if (numBytesSent < 0)
          DieWithSystemMessage("send() failed");
      else if (numBytesSent != len)
          DieWithUserMessage("send()", "sent unexpected number of bytes");

      close(clntSock);
  }
  close(servSock);
}
