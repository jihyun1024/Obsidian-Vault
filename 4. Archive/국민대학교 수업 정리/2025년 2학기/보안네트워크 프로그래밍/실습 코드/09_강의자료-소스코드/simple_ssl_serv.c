#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 9999

int main() {
    // OpenSSL 초기화
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    // TLS 서버용 컨텍스트 생성
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // 1) 서버 인증서 / 개인키 로드
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Failed to load server certificate\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Failed to load server private key\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    // 서버 인증서와 키가 매칭되는지 확인 (옵션)
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Server private key does not match the certificate public key\n");
        SSL_CTX_free(ctx);
        return 1;
    }

    // 2) 클라이언트 인증서 검증을 위한 CA 인증서 로드
    if (SSL_CTX_load_verify_locations(ctx, "ca.crt", NULL) <= 0) {
        fprintf(stderr, "Failed to load CA certificate for client verification\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    // 3) 상호 인증 설정: 클라이언트 인증서 필수
    SSL_CTX_set_verify(ctx,
                       SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       NULL);
    SSL_CTX_set_verify_depth(ctx, 4);

    // TCP 소켓 준비
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        SSL_CTX_free(ctx);
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        SSL_CTX_free(ctx);
        return 1;
    }

    if (listen(sockfd, 1) < 0) {
        perror("listen");
        close(sockfd);
        SSL_CTX_free(ctx);
        return 1;
    }

    printf("[Server] Listening on port %d...\n", PORT);

    int client = accept(sockfd, NULL, NULL);
    if (client < 0) {
        perror("accept");
        close(sockfd);
        SSL_CTX_free(ctx);
        return 1;
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client);

    // TLS 핸드셰이크 (클라이언트 인증서 포함)
    if (SSL_accept(ssl) <= 0) {
        fprintf(stderr, "[Server] SSL_accept failed\n");
        ERR_print_errors_fp(stderr);
    } else {
        printf("[Server] SSL handshake complete!\n");

        // 클라이언트 인증서 정보 출력(옵션)
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
            printf("[Server] No client certificate presented (should not happen due to SSL_VERIFY_FAIL_IF_NO_PEER_CERT)\n");
        }

        char buf[1024];
        int n = SSL_read(ssl, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("[Server] Received: %s\n", buf);
            SSL_write(ssl, "Hello from SSL server!\n", 31);
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
