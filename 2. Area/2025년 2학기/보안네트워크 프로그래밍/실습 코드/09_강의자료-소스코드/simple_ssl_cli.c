#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 9999
#define SERVER_IP "127.0.0.1"

int main() {
    // OpenSSL 초기화
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    // TLS 클라이언트 컨텍스트 생성
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // 1) 서버 인증서 검증을 위한 CA 인증서 로드
    if (SSL_CTX_load_verify_locations(ctx, "ca.crt", NULL) <= 0) {
        fprintf(stderr, "Failed to load CA certificate\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    // 2) 클라이언트 인증서 및 개인키 로드 (서버에 전송)
    if (SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Failed to load client certificate\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Failed to load client private key\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Client private key does not match the certificate public key\n");
        SSL_CTX_free(ctx);
        return 1;
    }

    // 3) 서버 인증서 검증 활성화
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);

    // TCP 소켓 생성 및 서버 연결
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        SSL_CTX_free(ctx);
        return 1;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("connect");
        close(sockfd);
        SSL_CTX_free(ctx);
        return 1;
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    // TLS 핸드셰이크 (서버 인증 + 클라이언트 인증서 전송)
    if (SSL_connect(ssl) <= 0) {
        fprintf(stderr, "[Client] SSL_connect failed\n");
        ERR_print_errors_fp(stderr);
    } else {
        printf("[Client] SSL connected!\n");

        // 서버 인증서 확인 (옵션)
        X509* server_cert = SSL_get_peer_certificate(ssl);
        if (server_cert) {
            char* subj = X509_NAME_oneline(X509_get_subject_name(server_cert), NULL, 0);
            char* issuer = X509_NAME_oneline(X509_get_issuer_name(server_cert), NULL, 0);
            printf("[Client] Server certificate subject: %s\n", subj);
            printf("[Client] Server certificate issuer : %s\n", issuer);
            OPENSSL_free(subj);
            OPENSSL_free(issuer);
            X509_free(server_cert);
        } else {
            printf("[Client] No server certificate received?!\n");
        }

        const char* msg = "Hello SSL Server!\n";
        SSL_write(ssl, msg, strlen(msg));

        char buf[1024];
        int n = SSL_read(ssl, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("[Client] Received: %s\n", buf);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
