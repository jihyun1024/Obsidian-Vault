#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SERVER_IP "127.0.0.1"
#define PORT 9999
#define N_THREADS 8

SSL_CTX* g_ctx = NULL;

void print_ssl_error(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    ERR_print_errors_fp(stderr);
}

SSL_CTX* create_client_ctx(void) {
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    // 서버 인증서 검증 + 클라이언트 인증서 제시
    if (SSL_CTX_load_verify_locations(ctx, "ca.crt", NULL) <= 0) {
        fprintf(stderr, "[Client] Failed to load CA certificate\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "[Client] Failed to load client certificate\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "[Client] Failed to load client private key\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "[Client] Private key does not match the certificate public key\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    // 서버 인증서 검증 활성화
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);

    return ctx;
}

void* client_thread(void* arg) {
    int sock;
    struct sockaddr_in server_addr;
    char msg[128], buf[128];
    int sleep_time;

    if (!g_ctx) {
        fprintf(stderr, "[Client %lu] SSL_CTX not initialized\n", pthread_self());
        pthread_exit(NULL);
    }

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

    // SSL 세션 생성 및 핸드셰이크
    SSL* ssl = SSL_new(g_ctx);
    if (!ssl) {
        print_ssl_error("[Client] SSL_new() failed");
        close(sock);
        pthread_exit(NULL);
    }

    if (SSL_set_fd(ssl, sock) == 0) {
        print_ssl_error("[Client] SSL_set_fd() failed");
        SSL_free(ssl);
        close(sock);
        pthread_exit(NULL);
    }

    if (SSL_connect(ssl) <= 0) {
        print_ssl_error("[Client] SSL_connect() failed");
        SSL_free(ssl);
        close(sock);
        pthread_exit(NULL);
    }

    // 서버 인증서 정보 확인 (옵션)
    X509* server_cert = SSL_get_peer_certificate(ssl);
    if (server_cert) {
        char* subj = X509_NAME_oneline(X509_get_subject_name(server_cert), NULL, 0);
        char* issuer = X509_NAME_oneline(X509_get_issuer_name(server_cert), NULL, 0);
        printf("[Client %lu] Server certificate subject: %s\n",
               (unsigned long)pthread_self(), subj);
        printf("[Client %lu] Server certificate issuer : %s\n",
               (unsigned long)pthread_self(), issuer);
        OPENSSL_free(subj);
        OPENSSL_free(issuer);
        X509_free(server_cert);
    }

    // 랜덤 시간 대기 이후 메시지 전송
    sleep_time = rand() % 5 + 1;
    sleep(sleep_time);

    snprintf(msg, sizeof(msg),
        "Hello from thread %lu after %d sec\n",
        (unsigned long)pthread_self(), sleep_time);

    if (SSL_write(ssl, msg, strlen(msg)) <= 0) {
        print_ssl_error("[Client] SSL_write() failed");
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(sock);
        pthread_exit(NULL);
    }

    int n = SSL_read(ssl, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("[Client %lu] Received: %s",
            (unsigned long)pthread_self(), buf);
    }
    else if (n == 0) {
        printf("[Client %lu] Server closed SSL connection.\n",
            (unsigned long)pthread_self());
    }
    else {
        int err = SSL_get_error(ssl, n);
        fprintf(stderr, "[Client %lu] SSL_read() error: %d\n",
            (unsigned long)pthread_self(), err);
        ERR_print_errors_fp(stderr);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[N_THREADS];

    // OpenSSL 초기화
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    g_ctx = create_client_ctx();
    if (!g_ctx) {
        fprintf(stderr, "Failed to create client SSL_CTX\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL));

    for (int i = 0; i < N_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, client_thread, NULL) != 0) {
            perror("pthread_create() failed");
        }
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[Client] All threads finished.\n");

    SSL_CTX_free(g_ctx);
    EVP_cleanup();
    return 0;
}
