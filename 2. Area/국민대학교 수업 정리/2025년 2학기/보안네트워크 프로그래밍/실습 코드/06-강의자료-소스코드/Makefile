# ---------------------------------------
# Makefile for TCP Echo Server/Client
# ---------------------------------------
# 이 Makefile은 두 개의 실행 파일을 만듭니다:
#   1. servex2 : TCP 서버
#   2. client  : TCP 클라이언트
#
# 사용법:
#   make          -> 서버와 클라이언트 모두 빌드
#   make servex2  -> 서버만 빌드
#   make client   -> 클라이언트만 빌드
#   make clean    -> 실행 파일 삭제
# ---------------------------------------

# 사용할 컴파일러
CC = gcc

# 컴파일 옵션 (-Wall은 모든 일반적인 경고 메시지를 출력)
CFLAGS = -Wall

# 기본 타깃: 서버와 클라이언트 모두 빌드
all: server client

# 서버 빌드 규칙
server: TCPServer4.c DieWithMessage.c Practical.h AddressUtility.c OTPlib.h OTPlib.c 
	$(CC) $(CFLAGS) -o server TCPServer4.c DieWithMessage.c AddressUtility.c OTPlib.c -lcrypto

# 클라이언트 빌드 규칙
client: TCPClient4.c DieWithMessage.c Practical.h AddressUtility.c OTPlib.h OTPlib.c 
	$(CC) $(CFLAGS) -o client TCPClient4.c DieWithMessage.c AddressUtility.c OTPlib.c -lcrypto 

# 생성된 실행 파일 삭제
clean:
	rm -f server client
