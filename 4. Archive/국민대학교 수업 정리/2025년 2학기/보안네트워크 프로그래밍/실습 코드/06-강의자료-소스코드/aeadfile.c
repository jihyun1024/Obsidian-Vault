#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#define IV_LEN 12
#define TAG_LEN 16
#define KEY_LEN 32
#define CHUNK 4096

static void hexprint(const char* label, const unsigned char* b, size_t n) {
    printf("%s:", label);
    for (size_t i = 0; i < n; ++i) printf(" %02x", b[i]);
    printf("\n");
}

int derive_key_from_password(const char* pwd, unsigned char key[KEY_LEN]) {
    unsigned int outlen = 0;
    if (!EVP_Digest((const unsigned char*)pwd, strlen(pwd), key, &outlen, EVP_sha256(), NULL)) return 0;
    return outlen == KEY_LEN;
}

int encrypt_file(const char* inpath, const char* outpath, const unsigned char key[KEY_LEN]) {
    FILE* inf = fopen(inpath, "rb");
    FILE* outf = fopen(outpath, "wb");
    
    EVP_CIPHER_CTX* ctx = NULL;
    int ret = 0;

    if (!inf || !outf) goto done;
    
    unsigned char iv[IV_LEN];

    if (1 != RAND_bytes(iv, sizeof(iv)))        goto done;
    if (fwrite(iv, 1, IV_LEN, outf) != IV_LEN)  goto done;
    if ((ctx = EVP_CIPHER_CTX_new()) == NULL)   goto done;

    // AES-GCM의 IV(Nonce) 길이를 기본값으로 할 경우, 
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv)) goto done;

    /* AES-GCM의 IV(Nonce) 길이를 명시적으로 설정하기 위해 다음과 같이 함
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) goto done;
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, NULL)) goto done;
    if (1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) goto gone;
    */
    
    unsigned char inbuf[CHUNK], outbuf[CHUNK + 16];
    int inlen, outlen;

    while ((inlen = fread(inbuf, 1, sizeof(inbuf), inf)) > 0) {
        if (1 != EVP_EncryptUpdate(ctx, outbuf, &outlen, inbuf, inlen)) goto done;
        if (outlen > 0) {
            if (fwrite(outbuf, 1, outlen, outf) != (size_t)outlen) goto done;
        }
    }
    if (ferror(inf)) goto done;

    if (1 != EVP_EncryptFinal_ex(ctx, outbuf, &outlen)) goto done;
    if (outlen > 0) if (fwrite(outbuf, 1, outlen, outf) != (size_t)outlen) goto done;

    unsigned char tag[TAG_LEN];
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, tag)) goto done;
    if (fwrite(tag, 1, TAG_LEN, outf) != TAG_LEN) goto done;

    ret = 1;
done:
    if(ctx != NULL)     EVP_CIPHER_CTX_free(ctx);
    if(inf != NULL)     fclose(inf); 
    if(outf != NULL)    fclose(outf);
    return ret;
}

int decrypt_file(const char* inpath, const char* outpath, const unsigned char key[KEY_LEN]) {
    EVP_CIPHER_CTX* ctx = NULL;
    int ret = 0;

    FILE* inf = fopen(inpath, "rb");
    FILE* outf = fopen(outpath, "wb");

    if (!inf || !outf) goto done;

    if (fseek(inf, 0, SEEK_END) != 0) goto done;
    long fsize = ftell(inf);
    if (fsize < IV_LEN + TAG_LEN) goto done;
    long ctext_len = fsize - IV_LEN - TAG_LEN;
    rewind(inf);

    unsigned char iv[IV_LEN];
    if (fread(iv, 1, IV_LEN, inf) != IV_LEN) goto done;
    if (fseek(inf, IV_LEN + ctext_len, SEEK_SET) != 0) goto done;

    unsigned char tag[TAG_LEN];
    if (fread(tag, 1, TAG_LEN, inf) != TAG_LEN) goto done;
    if (fseek(inf, IV_LEN, SEEK_SET) != 0) goto done;

    if((ctx = EVP_CIPHER_CTX_new()) == NULL) goto done;

    // AES-GCM의 IV(Nonce) 길이를 기본값으로 할 경우, 
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv)) goto done;
    /* AES-GCM의 IV(Nonce) 길이를 명시적으로 설정하기 위해 다음과 같이 함
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) goto done;
    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, NULL)) goto done;
    if (1 != EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) goto done; */

    // 내부적으로 패딩이나 인증 데이터 계산에 필요한 추가 공간을 쓸 수 있으므로
    // 출력 버퍼(outbuf)는 입력보다 약간 (16) 더 커야 합니다.
    unsigned char inbuf[CHUNK], outbuf[CHUNK + 16];
    long remaining = ctext_len;
    int inlen, outlen;

    while (remaining > 0) {
        int toread = (remaining > (long)sizeof(inbuf)) ? sizeof(inbuf) : (int)remaining;
        if (toread != (inlen = fread(inbuf, 1, toread, inf))) goto done;
        remaining -= inlen;
        if (1 != EVP_DecryptUpdate(ctx, outbuf, &outlen, inbuf, inlen)) goto done;
        if (outlen > 0) {
            if (fwrite(outbuf, 1, outlen, outf) != (size_t)outlen) goto done;
        }
    }

    if (1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, tag)) goto done;
    if (1 != EVP_DecryptFinal_ex(ctx, outbuf, &outlen)) goto done;
    if (outlen > 0) if (fwrite(outbuf, 1, outlen, outf) != (size_t)outlen) goto done;

   ret = 1;

done:
    if (ctx != NULL)     EVP_CIPHER_CTX_free(ctx);
    if (inf != NULL)     fclose(inf);
    if (outf != NULL)    fclose(outf);
    return ret;
}

int main(int argc, char** argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s enc|dec <password> <infile> <outfile>\n", argv[0]);
        return 1;
    }
    ERR_load_crypto_strings(); OpenSSL_add_all_algorithms();

    const char* mode = argv[1];
    const char* pwd = argv[2];
    const char* infile = argv[3];
    const char* outfile = argv[4];

    unsigned char key[KEY_LEN];
    if (!derive_key_from_password(pwd, key)) { fprintf(stderr, "derive key fail\n"); return 1; }

    if (strcmp(mode, "enc") == 0) {
        if (!encrypt_file(infile, outfile, key)) { fprintf(stderr, "encryption failed\n"); return 1; }
        printf("Encrypted %s -> %s\n", infile, outfile);
    }
    else if (strcmp(mode, "dec") == 0) {
        if (!decrypt_file(infile, outfile, key)) { fprintf(stderr, "decryption/verification failed\n"); return 1; }
        printf("Decrypted %s -> %s\n", infile, outfile);
    }
    else {
        fprintf(stderr, "mode must be enc or dec\n");
        return 1;
    }

    EVP_cleanup(); ERR_free_strings();
    return 0;
}