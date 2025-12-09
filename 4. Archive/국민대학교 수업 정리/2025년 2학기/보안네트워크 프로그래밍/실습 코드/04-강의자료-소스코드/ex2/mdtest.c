#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>

/* 헥사 출력 보조함수 */
static void hexprint(const unsigned char* buf, size_t len) {
    for (size_t i = 0; i < len; ++i) printf("%02x", buf[i]);
    printf("\n");
}

/* 1) 단일 호출(one-shot) 방식: 메모리 상의 버퍼 해시 */
void hash_one_shot(const unsigned char* data, size_t datalen) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;

    /* 편의형 단일 호출 */
    if (!EVP_Digest(data, datalen, digest, &digest_len, EVP_sha256(), NULL)) {
        fprintf(stderr, "EVP_Digest failed\n");
        return;
    }

    printf("One-shot SHA-256: ");
    hexprint(digest, digest_len);
}

/* 2) 증분 방식(incremental): 스트림/큰 파일에 유용 */
int hash_stream_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        return -1;
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        fprintf(stderr, "EVP_MD_CTX_new failed\n");
        fclose(f);
        return -1;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        fprintf(stderr, "EVP_DigestInit_ex failed\n");
        EVP_MD_CTX_free(mdctx);
        fclose(f);
        return -1;
    }

    unsigned char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (EVP_DigestUpdate(mdctx, buf, n) != 1) {
            fprintf(stderr, "EVP_DigestUpdate failed\n");
            EVP_MD_CTX_free(mdctx);
            fclose(f);
            return -1;
        }
    }
    if (ferror(f)) {
        perror("fread");
        EVP_MD_CTX_free(mdctx);
        fclose(f);
        return -1;
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    if (EVP_DigestFinal_ex(mdctx, digest, &digest_len) != 1) {
        fprintf(stderr, "EVP_DigestFinal_ex failed\n");
        EVP_MD_CTX_free(mdctx);
        fclose(f);
        return -1;
    }

    printf("File SHA-256 (%s): ", filename);
    hexprint(digest, digest_len);

    EVP_MD_CTX_free(mdctx);
    fclose(f);
    return 0;
}

int main(int argc, char** argv) {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    const char* msg = "Hello, OpenSSL EVP SHA-256!";
    hash_one_shot((const unsigned char*)msg, strlen(msg));

    if (argc >= 2)
        hash_stream_file(argv[1]);
    else
        printf("Usage: %s <file-to-hash>\n", argv[0]);
   
    /* 정리 */
    EVP_cleanup();
    ERR_free_strings();
    return 0;
}