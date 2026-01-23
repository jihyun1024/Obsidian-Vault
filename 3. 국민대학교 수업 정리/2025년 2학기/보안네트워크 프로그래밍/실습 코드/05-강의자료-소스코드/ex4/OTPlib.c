#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <openssl/opensslv.h>
#include <openssl/crypto.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/err.h>

#include "Practical.h"
#include "OTPlib.h"

bool calc_otp(char* pwd, char* otp, int* otplen)
{
    if (pwd == NULL || otp == NULL || otplen == NULL)
        DieWithUserMessage("calc_otp() failed", "invalid input parameter - pwd, otp, otplen");

    unsigned char   key[EVP_MAX_MD_SIZE];
    unsigned int    key_len = 0, len = 0;

    len = strlen(pwd);
    if (!EVP_Digest(pwd, len, key, &key_len, EVP_sha256(), NULL))
        DieWithUserMessage("calc_otp() failed", "EVP_Digest failed");

    time_t t = floor((time(NULL) - t0) / OTP_STEP);
    if (NULL == HMAC(EVP_sha256(), key, (int)key_len, (const unsigned char*)&t, sizeof(t), 
                     (unsigned char*)otp, (unsigned int*)otplen))
        return false;
    return true;
}

bool verify_otp(char* pwd, char* otp, int otplen)
{
    char d_otp[EVP_MAX_MD_SIZE];
    int len = 0;

    memset(d_otp, 0, EVP_MAX_MD_SIZE);
    if(!calc_otp(pwd, d_otp, &len))
        return false;
    else if(len != otplen)
        return false;

    return (0 == CRYPTO_memcmp(d_otp, otp, (size_t) len));
}
