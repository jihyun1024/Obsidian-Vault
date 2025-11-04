#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define N_THREADS 8

void* client_thread(void* arg) {
    int sock;
    struct sockaddr_in server_addr;
    char msg[128], buf[128];
    int sleep_time;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton() failed");
        close(sock);
        pthread_exit(NULL);
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        close(sock);
        pthread_exit(NULL);
    }

    sleep_time = rand() % 5 + 1;  // 1~5초 랜덤 sleep
    sleep(sleep_time);

    snprintf(msg, sizeof(msg), "Hello from thread %lu after %d sec\n",
        pthread_self(), sleep_time);

    if (send(sock, msg, strlen(msg), 0) < 0) {
        perror("send() failed");
        close(sock);
        pthread_exit(NULL);
    }

    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
        buf[n] = '\0';
        printf("[Client %lu] Received: %s", pthread_self(), buf);
    }
    else if (n == 0) {
        printf("[Client %lu] Server closed connection.\n", pthread_self());
    }
    else {
        perror("recv() failed");
    }

    close(sock);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N_THREADS];
    srand(time(NULL));

    for (int i = 0; i < N_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, client_thread, NULL) != 0) {
            perror("pthread_create() failed");
        }
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[Client] All threads finished.\n");
    return 0;
}