#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/opensslv.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/err.h>

static void hexprint(const unsigned char* buf, size_t len) {
    for (size_t i = 0; i < len; ++i) printf("%02x", buf[i]);
    printf("\n");
}

bool hmac_file(const char* filename, const char* key, size_t keylen, unsigned char* out, size_t* outlen) {
    if (filename == NULL || key == NULL || keylen < 0 ||
        out == NULL || outlen == NULL) return false;
    
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        return false;
    }

    bool            ret = false;
    EVP_MAC*        mac = NULL;
    EVP_MAC_CTX*    mctx = NULL;
    unsigned char   buf[4096];
    size_t          n;

    mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
    if (!mac) goto done;

    mctx = EVP_MAC_CTX_new(mac);
    if (!mctx) goto done;

    OSSL_PARAM params[2];
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_ALG_PARAM_DIGEST, (char*)"SHA256", 0);
    params[1] = OSSL_PARAM_construct_end();

    if (!EVP_MAC_init(mctx, key, keylen, params)) goto done;
    memset(buf, 0, sizeof(buf));
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (EVP_MAC_update(mctx, buf, n) != 1) { // 반환값이 1일 경우 성공
            fprintf(stderr, "EVP_MAC_update failed\n");
            goto done;
        }
    }
    if (!EVP_MAC_final(mctx, out, outlen, EVP_MAX_MD_SIZE)) goto done;
    ret = true;
done:
    if (mctx != NULL) EVP_MAC_CTX_free(mctx);
    if (mac != NULL) EVP_MAC_free(mac);
    if (f != NULL) fclose(f);
    return ret;
   }

int main(int argc, char** argv) {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    if (argc < 3){
        printf("Usage: %s <file-for-hmac> <key>\n", argv[0]);
        goto done;
    }

    unsigned char out[EVP_MAX_MD_SIZE];
    size_t outlen;

    memset(out, 0, sizeof(out));
    if(hmac_file(argv[1], argv[2], strlen(argv[2]), out, &outlen))
    {
        printf("File HMAC-SHA-256 (%s): ", argv[1]);
        hexprint(out, outlen);
    }
    else
        printf("Failed to calculate HMAC\n");
done:
    EVP_cleanup();
    ERR_free_strings();
    return 0;
}