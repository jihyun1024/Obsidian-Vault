#ifndef OTPLIB_H_
#define OTPLIB_H_


#define t0          0
#define OTP_STEP    30


bool calc_otp(char* pwd, char* otp, int* otplen);
bool verify_otp(char* pwd, char* otp, int otplen);

#endif