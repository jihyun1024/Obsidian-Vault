#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/opensslv.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/err.h>

static void hexprint(const unsigned char* p, size_t len) {
    for (size_t i = 0; i < len; ++i) printf("%02x", p[i]);
    printf("\n");
}

int hmac_one_shot_evp(const unsigned char* key, size_t keylen,
    const unsigned char* data, size_t datalen,
    unsigned char* out, size_t* outlen, size_t outsize)
{
    unsigned int ulen = 0;
    unsigned char* r = HMAC(EVP_sha256(), key, (int)keylen, data, datalen, out, &ulen);
    if (!r) return 0;
    *outlen = (size_t)ulen;
    (void)outsize;
    return 1;

#ifdef NEVER
    int ok = 0;
    EVP_MAC* mac = NULL;
    EVP_MAC_CTX* mctx = NULL;

    mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
    if (!mac) goto done;

    mctx = EVP_MAC_CTX_new(mac);
    if (!mctx) goto done;

    /* set digest = "SHA256" */
    OSSL_PARAM params[2];
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_ALG_PARAM_DIGEST, (char*)"SHA256", 0);
    params[1] = OSSL_PARAM_construct_end();

    if (!EVP_MAC_init(mctx, key, keylen, params)) goto done;

    if (!EVP_MAC_update(mctx, data, datalen)) goto done;

    if (!EVP_MAC_final(mctx, out, outlen, outsize)) goto done;

    ok = 1;

done:
    if (mctx) EVP_MAC_CTX_free(mctx);
    if (mac) EVP_MAC_free(mac);
    return ok;
#endif
}

int hmac_stream_evp(const unsigned char* key, size_t keylen,
    const unsigned char** chunks, const size_t* chunk_lens, size_t n_chunks,
    unsigned char* out, size_t* outlen, size_t outsize)
{
    int ok = 0;
    EVP_MAC* mac = NULL;
    EVP_MAC_CTX* mctx = NULL;

    mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
    if (!mac) goto done;

    mctx = EVP_MAC_CTX_new(mac);
    if (!mctx) goto done;

    OSSL_PARAM params[2];
    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_ALG_PARAM_DIGEST, (char*)"SHA256", 0);
    params[1] = OSSL_PARAM_construct_end();

    if (!EVP_MAC_init(mctx, key, keylen, params)) goto done;

    for (size_t i = 0; i < n_chunks; ++i) {
        if (!EVP_MAC_update(mctx, chunks[i], chunk_lens[i])) goto done;
    }

    if (!EVP_MAC_final(mctx, out, outlen, outsize)) goto done;

    ok = 1;

done:
    if (mctx) EVP_MAC_CTX_free(mctx);
    if (mac) EVP_MAC_free(mac);
    return ok;
}

int main(void)
{
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    const unsigned char key[] = "demo_key_for_hmac_example_32bytes__";
    const unsigned char msg1[] = "Hello ";
    const unsigned char msg2[] = "OpenSSL!";

    unsigned char mac[EVP_MAX_MD_SIZE];
    size_t maclen = 0;

    if (!hmac_one_shot_evp(key, strlen((char*)key), msg1, strlen((char*)msg1), mac, &maclen, sizeof(mac))) {
        fprintf(stderr, "one-shot HMAC failed\n");
        return 1;
    }
    printf("one-shot HMAC-SHA256: "); hexprint(mac, maclen);

    const unsigned char* chunks[2] = { msg1, msg2 };
    const size_t chunk_lens[2] = { strlen((char*)msg1), strlen((char*)msg2) };

    if (!hmac_stream_evp(key, strlen((char*)key), chunks, chunk_lens, 2, mac, &maclen, sizeof(mac))) {
        fprintf(stderr, "streaming HMAC failed\n");
        return 1;
    }
    printf("stream HMAC-SHA256:    "); hexprint(mac, maclen);

    EVP_cleanup();
    ERR_free_strings();
    return 0;
}