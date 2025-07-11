어셈블리어는 같은 인텔 CPU에서도 Intel 기반 표기 방법과 AT&T 기반 표기 방법으로 나눠진다. 
(이 문서에서는 Intel 기반 표기를 따른다)

### 어셈블리어 명령어
[[데이터 이동 명령어]]
[[산술 연산 명령어]]
[[비트 연산 명령어]]
[[제어 명령어]]
[[분기 명령어]]
[[루프 명령어]]
[[다형성 코드]]

### Hello, world! 출력 코드 예시
```
adosseg
	.model small
	.stack 100h

.data
hello_message db 'Hell0, world!', 0dh, 0ah, '$'

.code
main proc
	mov ax, @data
	mov ds, ax
	mov ah, 9
	mov dx, offset hello_message
	int 21h

	mov ax, 4000h
	int 21h
main endp
end main
```
