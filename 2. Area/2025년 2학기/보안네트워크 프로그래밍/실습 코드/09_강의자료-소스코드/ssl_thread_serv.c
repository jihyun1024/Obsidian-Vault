#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 9999
#define BUF_SIZE 1024

typedef struct {
    int client_sock;
    SSL* ssl;
} client_info_t;

void print_ssl_error(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    ERR_print_errors_fp(stderr);
}

void* client_handler(void* arg) {
    client_info_t* info = (client_info_t*)arg;
    int client_sock = info->client_sock;
    SSL* ssl = info->ssl;
    free(info);

    char buf[BUF_SIZE];
    int n;

    // TLS 핸드셰이크 (클라이언트 인증 포함)
    if (SSL_accept(ssl) <= 0) {
        print_ssl_error("[Server] SSL_accept() failed");
        goto cleanup;
    }

    printf("[Server] SSL handshake completed.\n");

    // 클라이언트 인증서 정보 출력 (옵션)
    X509* client_cert = SSL_get_peer_certificate(ssl);
    if (client_cert) {
        char* subj = X509_NAME_oneline(X509_get_subject_name(client_cert), NULL, 0);
        char* issuer = X509_NAME_oneline(X509_get_issuer_name(client_cert), NULL, 0);
        printf("[Server] Client certificate subject: %s\n", subj);
        printf("[Server] Client certificate issuer : %s\n", issuer);
        OPENSSL_free(subj);
        OPENSSL_free(issuer);
        X509_free(client_cert);
    } else {
        printf("[Server] No client certificate presented (should not happen).\n"); }

    // SSL 기반 통신 루프
    for (;;) {
        n = SSL_read(ssl, buf, BUF_SIZE - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("[Server] Received: %s", buf);

            if (SSL_write(ssl, "ACK\n", 4) <= 0) {
                print_ssl_error("[Server] SSL_write() failed");
                break;
            }
        }
        else {
            if (n == 0) {
                printf("[Server] Client disconnected (SSL_read returned 0).\n");
            }
            else {
                int err = SSL_get_error(ssl, n);
                fprintf(stderr, "[Server] SSL_read() error: %d\n", err);
                ERR_print_errors_fp(stderr);
            }
            break;
        }
    }

cleanup:
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    close(client_sock);
    pthread_exit(NULL);
}

// 서버용 SSL_CTX 생성
SSL_CTX* create_server_ctx(void) {
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    // 서버 인증서와 개인키 로드
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the certificate public key\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    // 클라이언트 인증서 검증을 위한 CA 인증서 로드
    if (SSL_CTX_load_verify_locations(ctx, "ca.crt", NULL) <= 0) {
        fprintf(stderr, "[Server] Failed to load CA certificate\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return NULL;
    }

    // 클라이언트 인증 필수 설정
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);

    return ctx;
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;

    // OpenSSL 초기화
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX* ctx = create_server_ctx();
    if (!ctx) {
        fprintf(stderr, "Failed to create SSL_CTX\n");
        exit(EXIT_FAILURE);
    }

    // 소켓 생성
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    // 주소 재사용 옵션(선택)
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        close(server_sock);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 8) < 0) {
        perror("listen() failed");
        close(server_sock);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on port %d ...\n", PORT);

    // 클라이언트 접속 루프
    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("accept() failed");
            continue;
        }

        printf("[Server] Client connected from %s:%d\n",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        SSL* ssl = SSL_new(ctx);
        if (!ssl) {
            ERR_print_errors_fp(stderr);
            close(client_sock);
            continue;
        }

        if (SSL_set_fd(ssl, client_sock) == 0) {
            print_ssl_error("[Server] SSL_set_fd() failed");
            SSL_free(ssl);
            close(client_sock);
            continue;
        }

        client_info_t* info = (client_info_t*)malloc(sizeof(client_info_t));
        if (!info) {
            fprintf(stderr, "malloc() failed\n");
            SSL_free(ssl);
            close(client_sock);
            continue;
        }
        info->client_sock = client_sock;
        info->ssl = ssl;

        if (pthread_create(&tid, NULL, client_handler, info) != 0) {
            perror("pthread_create() failed");
            SSL_free(ssl);
            close(client_sock);
            free(info);
        }
        else {
            pthread_detach(tid);  // 종료 시 스레드 자원 자동 해제
        }
    }

    close(server_sock);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}
