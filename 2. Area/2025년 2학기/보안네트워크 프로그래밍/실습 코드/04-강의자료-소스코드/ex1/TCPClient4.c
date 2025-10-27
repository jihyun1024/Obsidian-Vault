#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

void send_msg(int sock, char* id, char* pwd)
{
    if(sock < 0 || id == NULL || pwd == NULL) 
        DieWithUserMessage("send_msg() failed", "invalid input parameters - socket descryptor, id, pwd");
    
    char buffer[BUFSIZE];
    int len;
    int numBytes;
    
    memset(buffer, 0, BUFSIZE);

    sprintf(buffer, "%s;%s", id, pwd);
    len = strlen(buffer);

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
  fputs("\nReceived: ", stdout);
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
