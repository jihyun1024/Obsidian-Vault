#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8080
#define BUF_SIZE 1024

void* client_handler(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);

    char buf[BUF_SIZE];
    ssize_t n;

    while ((n = recv(client_sock, buf, BUF_SIZE - 1, 0)) > 0) {
        buf[n] = '\0';
        printf("[Server] Received: %s", buf);

        if (send(client_sock, "ACK\n", 4, 0) < 0) {
            perror("send() failed");
            break;
        }
    }

    if (n == 0) {
        printf("[Server] Client disconnected.\n");
    }
    else if (n < 0) {
        perror("recv() failed");
    }

    close(client_sock);
    pthread_exit(NULL);
}

int main() {
    int server_sock, * client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;

    // 소켓 생성
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // 주소 재사용 옵션
    //int opt = 1;
    //setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 바인드
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 리슨
    if (listen(server_sock, 8) < 0) {
        perror("listen() failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on port %d...\n", PORT);

    // 클라이언트 접속 루프
    while (1) {
        client_sock = malloc(sizeof(int));
        if (!client_sock) {
            fprintf(stderr, "malloc() failed\n");
            continue;
        }

        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (*client_sock < 0) {
            perror("accept() failed");
            free(client_sock);
            continue;
        }

        printf("[Server] Client connected from %s:%d\n",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if (pthread_create(&tid, NULL, client_handler, client_sock) != 0) {
            perror("pthread_create() failed");
            close(*client_sock);
            free(client_sock);
        }
        else {
            pthread_detach(tid);  // 종료시 스레드가 자동 해제되도록 함
        }
    }

    close(server_sock);
    return 0;
}