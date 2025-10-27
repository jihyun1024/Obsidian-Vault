#ifndef OTPLIB_H_
#define OTPLIB_H_

#define OTP_CHAL_SIZE  16
#define OTP_RESP_SIZE  32
#define OTP_STEP	   30
#define AES128_KEY_LEN 16
#define AES_BLOCK_LEN  16

void derive_key(const char* pwd, unsigned char key_out[AES128_KEY_LEN]);
bool calc_otp(char* pwd, char* resp, char* otp, int* otplen);
bool verify_otp(char* pwd, char* resp, char* otp, int otplen);

int aes_encrypt(const unsigned char *plaintext, int plaintext_len,
                const unsigned char key[AES128_KEY_LEN], const unsigned char iv[AES_BLOCK_LEN],
                unsigned char *ciphertext);

int aes_decrypt(const unsigned char *ciphertext, int ciphertext_len,
                const unsigned char key[AES128_KEY_LEN], const unsigned char iv[AES_BLOCK_LEN],
                unsigned char *plaintext);


#endif