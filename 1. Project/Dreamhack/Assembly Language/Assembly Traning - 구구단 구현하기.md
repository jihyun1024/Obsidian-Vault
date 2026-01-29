### 들어가며
---
이번 강의에서는 이용자의 입력을 받아 **구구단**을 출력하는 프로그램을 어셈블리어로 작성해 본다. `read` 시스템 콜로 이용자에게 문자열을 입력받고, 입력받은 문자열을 숫자로 변환한 후, 반복문과 조건문을 활용해 변환한 숫자와 1부터 9까지의 곱셈 연산 결과를 출력한다. 

단순히 어셈블리 명령어와 문법을 암기하는 것을 넘어서, 실제 프로그램 구현을 통해 다양한 활용 방법을 익힐 수 있도록 작성했으니, 차근차근 따라오며 실습해 보자.


### 프로그램 기획
---
구구단 프로그램을 작성하기 위해서는 아래의 기능을 구현해야 한다. 

1. **이용자 입력 처리**
	- 숫자 입력: 이용자에게 계산하고자 하는 숫자를 문자열로 입력받음
	- 문자열 정수 변환: 이용자에게 입력받은 문자열을 정수로 변환
	- 예외 처리: 정수가 1보다 작거나 99보다 큰 경우, 프로그램을 종료
	
2. **구구단 출력**
	- 곱셈 및 출력: 입력된 정수와 곱해지는 수를 1~9까지 변경하며 곱한 후, 연산 결과 출력
	- 9회 반복 루프: 위의 '곱셈 및 출력' 과정을 9회 연속으로 수행

본 강의에서 구현하는 프로그램의 최종 출력은 아래와 같다.
![[Pasted image 20251218092742.png]]

구현에 들어가기 전, 앞서 살펴본 기능들에 대한 세부 구현을 살펴본다.

**1. 반복문**
해당 프로그램은 곱셈과 결과 값 출력을 각각 9번씩 반복한다. 따라서 반복문 구현이 필수적이며, 이는 분기문과 조건문(`cmp`, `jmp`, `je`)으로 구현할 수 있다. 

**2. 개행 문자(`\n`) 처리**
일반적으로 컴퓨터는 이용자 입력을 문자열로 처리한다. 이때 `read` 시스템 콜을 사용해 이용자로부터 문자열을 입력받는 경우, 입력의 끝을 나타내는 개행 문자(`\n`)가 문자열의 마지막에 같이 저장된다. 이는 나중에 문자열을 정수로 변환할 때 번거로움이 생길 수 있어, 개행 문자를 널 바이트로 바꿔주는 것이 좋다. 

이는 **1.** 과 유사하게 분기문과 조건문을 활용해서 반복문으로 구현할 수 있다. 각각의 바이트를 순회하며 개행 문자와 동일한지 비교하고, 개행 문자인 경우 이를 널 바이트로 치환한다. 

**3. 문자열의 정수 변환**
사용자가 숫자를 입력하면 컴퓨터는 해당 숫자를 문자열로 인식하기 때문에, 이 문자열을 정수 데이터로 변환하기 위해 C 언어의 `atoi()`와 비슷한 기능을 직접 구현해야 한다. 

문자 '9'를 정수 9로 변환하는 과정을 생각해 보자. 문자 '9'는 아스키 코드로 57에 해당한다. 이때 문자 '0'은 아스키 코드 48에 해당하므로, 문자 '9'의 아스키 코드에서 문자 '0'의 아스키 코드를 뺀 값인 정수 9를 얻을 수 있다. 따라서 모든 숫자 문자에 대해 해당 문자의 아스키 코드에서 기준점이 되는 '0'의 아스키 코드인 48을 빼면, 대응하는 정수 값을 구할 수 있다.

이제 문자열 '9876'을 정수 9876으로 변환하는 과정을 생각해 보자. 우선 각 자릿수의 숫자 문자를 정수로 변환한 후, 10의 거듭제곱을 통해 정수로 변환할 수 있다. 이를 설명하기 위해 정수 9876을 십진법의 전개식으로 나타내면 다음과 같다. 
$$9876 = 9 \times 10^3 + 8 \times 10^2 + 7 \times 10^1 + 6 \times 10^0$$

그러나, 연산의 편의를 위해선 위의 직관적인 계산식을 사용하는 것보다 **호너의 법칙**을 사용하는 것이 더 효율적이다. 따라서 본 강의에서는 각각의 문자를 정수 9, 8, 7, 6으로 변환한 후, 호너의 법칙으로 문자열을 정수로 변환한다. 호너의 법칙으로 정수 9876을 나타낸 식은 다음과 같다.
$$9876 = ((9 \times 10 + 8) \times 10 + 7) \times 10 + 6$$
>[!example] 호너의 법칙이란?
>**호너의 법칙(Horner's rule)** 또는 **호너의 방법(Horner's method)** 은 다항식을 표현하는 방법 중 하나로, 다항식을 중첩된 형태로 바꿔 한 번의 순회로 계산할 수 있게 만드는 기법이다. 이 방법을 이용하면 지수 연산을 명시적으로 수행하지 않고도 계수를 차례대로 곱셈, 덧셈해 나가면서 최종 값을 얻을 수 있어 메모리 효율성이 좋다. 아래는 다항식 $p(x)$를 호너의 법칙으로 나타내는 예시이다.
>$$p(x) = a_0 + a_1x + a_2x^2+...+a_nx^n = \
> a_0 + x(a_1 +x(a_2 + x(...+x(a_{n-1} + xa_n)...)))$$

계산식 출력에는 `printf()`와 포맷 스트링을 사용한다. 이때 `printf()`는 구현이 번거롭기 때문에 `extern` 명령어를 통해 외부에 정의된 함수를 사용한다. 다음 토픽부터는 구현된 코드를 설명하므로, 만약 먼저 구현해 보고 싶다면 충분히 구현해 본 뒤 넘어가자.


### 구현 - .data/.bss 섹션 정의
---
프로그램의 기능을 구현하기 전, 입출력에 사용될 문자열 데이터와 상수를 정의해야 한다. 데이터 세그먼트(Data segment)에는 값이 정해진, 즉 초기화가 된 전역 변수와 상수들이 위치한다. 따라서, `.data` 섹션에 **프로그램에서 사용될 상수 문자열들과 전역 변수 및 상수를 정의**한다. 일반적으로 상수 문자열은 `.rodata` 섹션에 정의되지만, 본 강의에서는 실습의 편의를 위해 싹 다 몰아서 `.data` 섹션에 정의한다. 

C 에서 따로 변수 초기화를 해 주는 것처럼, 어셈블리어에선 `section`을 통해 특정 섹션에 데이터를 정의할 수 있다. 아래는 `section .data`로 `.data`에 상수와 문자열을 정의하는 코드이다.
```nasm
section .data
	prompt_msg        db "Enter a number (1~99): ", 0
	prompt_msg_len    equ $ - prompt_msg
	
	invalid_msg       db "Invalid number! (1 <= number <= 99)", 10, 0
	invalid_msg_len   equ $ - invalid_msg
	
	invalid_digit_msg db "Error: Invalid digit in input for given base.", 10, 0
	invalid_digit_msg_len equ $ -invalid_digit_msg - 1
	
	fmt_mul        db "%d * %d = %d", 10, 0
	
	STDIN:        equ 0
	STDOUT:       equ 1
	SYS_READ:     equ 0
	SYS_WRITE:    equ 1
	SYS_EXIT:     equ 60
	
	MAX_LEN:      equ 3
	EOL_CHAR      equ 10
```

- `prompt_msg`, `prompt_msg_len`: 숫자를 입력받기 전 출력할 메시지와 그 길이
- `invalid_msg`, `invalid_msg_len`: 범위 밖의 숫자가 입력된 경우 출력할 메시지와 그 길이
- `invalid_digit_msg`, `invalid_digit_msg_len`: 문자열을 정수로 변환하는 데 실패했을 때 출력할 메시지와 메시지의 길이
- `fmt_mul`: `printf()`의 첫 번째 인자로 들어갈 포맷 스트링(Format string)
- `STDIN`, `STDOUT`: 표준 입출력의 파일 디스크립터(File descriptor)
- `SYS_READ`, `SYS_WRITE`, `SYS_EXIT`: x86-64 환경에서의 `read`, `write`, `exit` 시스템 콜 번호
- `MAX_LEN`: 이용자가 입력할 수 있는 문자열의 최대 길이로, 1부터 99 사이의 숫자를 입력받을 것이니 *개행 문자를 포함해* 최대 세 자리까지 입력
- `EOL_CHAR`: 개행 문자의 아스키 코드 값

그 다음, 변수로 사용할 데이터는 `.bss` 섹션에 정의한다. 초기 값이 정해져 있지 않은 변수들은 `.bss` 섹션에 할당된다. `.data` 섹션과 유사하게 `section .bss`로 각 변수를 정의한다. 

```nasm 
section .bss
	input_str:    resb MAX_LEN
	input_num:    resb 1
```

- `input_str`: 이용자에게 문자열을 입력받을 변수
- `input_num`: 숫자 문자열을 정수로 변환한 결과를 저장할 변수

>[!example] `db`, `equ`, `resb`는? 
>- `db`(Define Byte): 1 바이트 단위로 데이터를 정의하는 키워드
>- `equ`(Equate): 각 심볼에 상수 값을 대입해 정의하는 키워드
>- `resb`(Reserve Byte): 1 바이트 단위로 메모리 공간을 예약하는 키워드


### 이용자 입력 처리
---
본격적으로 프로그램을 구현해 보자. 프로그램이 시작되면 이용자로부터 문자열을 전달받고, 이를 정수로 변환해야 한다. C 언어에서는 `scanf()`와 형식 지정자 `%d`를 통해 이용자에게 정수를 바로 입력받을 수 있지만, 어셈블리어로 프로그램을 작성할 때는 문자열을 정수로 변환하는 과정을 직접 구현해야 한다. 

Linux에서는 `read` 시스템 콜을 통해 이용자로부터 데이터를 문자열 형태로 입력받을 수 있다. 따라서 문자열을 정수로 변환하는 `parse_number()`와, 개행 문자를 널 바이트로 치환하는 `strip_eol()`을 구현해야 한다. 문자열을 변환하기 전 개행 문자를 널 바이트로 치환하므로, 먼저 `strip_eol()`부터 구현해 보자. 

**`strip_eol()` 구현**
`strip_eol()` 함수는 조건문과 분기문을 사용해 반복문을 수행하고, 각 바이트를 순회하며 개행 문자를 만나면 널 바이트로 치환하는 함수이다. 

기능을 구현하기 전, 어셈블리어에서 반복문을 구현하는 방법을 알아 보자. 아래 코드는 임의의 동작을 10번 수행하는 반복문을 어셈블리어로 작성한 것이다. 참고로, 어셈블리어에서는 세미콜론을 이용해 주석을 작성할 수 있다. 

```nasm
	xor    rcx, rcx
.loop:
	cmp    rcx, 10
	je     end_loop  ; rcx가 10인 경우, .end_loop 레이블로 분기
	...          ; 임의 동작 수행
	inc    rcx      ; rcx값 1 증가
	jmp    .loop    ; .loop 레이블로 무조건 점프
.end_loop
	...          ; 반복문이 종료된 후 실행할 코드
```

아래의 코드는 위에서 작성한 반복문의 형식을 응용해 `strip_eol()`을 작성한 것이다. 이 반복문은 사용자의 입력을 저장하는 버퍼의 크기인 `MAX_LEN`만큼 반복문을 순회하며, 개행 문자를 찾아 널 바이트로 치환한다. 반복문의 종료 조건에 `MAX_LEN`을 추가하지 않을 경우, 개행 문자를 만날 때까지 배열 범위를 벗어나 비교를 계속 수행해 **OOB(Out-Of Bound)** 가 발생할 수 있다. 

`strip_eol()`은 각 바이트를 순회하며 개행 문자를 만난 경우 `.replace_eol`레이블로 분기하며, 각 바이트에 대한 순회가 모두 끝나거나 개행 문자를 만난 경우 `.done_strip`로 분기한다.

```nasm
strip_eol:
	xor     rcx, rcx ; 
.strip_loop:
	cmp     rcx, MAX_LEN
	jge     .done_strip
	cmp     byte [rdi+rcx], EOL_CHAR
	je      .replace_eol
	inc     rcx
	jmp     .strip_loop
.replace_eol
	mov     byte [rdi+rcx], 0
.done_strip
	ret
```

**`parse_number()` 구현**
사용자로부터 입력받은 문자열을 정수로 변환하는 함수인 `parse_number()`를 구현해 보자. 앞서 살펴봤듯, 이 과정에서 문자열을 효율적으로 처리하기 위해 거듭제곱 연산 대신 **호너의 법칙**을 사용한다. 호너의 법칙은 *앞에서부터 문자를 하나씩 읽으며 연산하는 방식*으로 동작하기 때문에, 거듭제곱 연산 없이 한 번의 순회만으로 값을 계산할 수 있다는 장점이 있다. 

아래는 `parse_number()`를 구현한 코드로, 호너의 법칙을 사용해 문자열을 정수로 변환한다. 일반적으로 함수의 반환 값은 `rax`에 저장되므로 `parse_number()` 또한 계산 결과를 `rax`에 누적하는 방식으로 구현한다. 첫 번째 인자인 `rdi`에는 변환하고자 하는 문자열의 주소가, 두 번째 인자인 `rsi`에는 10이 전달되어 문자열을 십진수로 변환한다. 

```nasm
parse_number:
	xor     rax, rax ; 반환 값 저장을 위한 rax 레지스터 초기화
	mov     r8, rdi ; r8에 변환하고자 하는 문자열 주소 저장
.parse_loop:
	mov     dl, [r8] 
	cmp     dl, 0
	je      .parse_done ; 문자열의 끝을 만난 경우, 반복문 종료
.digit_num:
	sub     dl, '0'
	movzx   ecx, dl
	mul     rsi ; rax *= rsi
	add     rax, rcx ; rax += rcx
	inc     r8 ; 타겟 문자열 포인터 1 증가
	jmp     .parse_loop 
.parse_done
	ret
```

또한, 입력된 문자의 아스키 코드가 '0'보다 작거나 '9'보다 클 경우, *변환에 실패했다는 에러 메시지를 출력하고 프로그램을 종료*하는 `exit_program()`으로 점프한다. 

```nasm
.parse_loop:
	mov     dl, [r8]
	cmp     dl, 0
	je      .parse_done
	cmp     dl, '0'
	jb      .parse_error
	cmp     dl, '9'
	jbe     .digit_num
	jmp     .parse_error
	...
.parse_error
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [invalid_digit_msg]
	mov     rdx, invalid_digit_msg_len
	syscall
	jmp     exit_program
```

예외 처리 과정을 포함한 `parse_number()`의 전체 코드는 다음과 같다. 

```nasm
parse_number:
	xor     rax, rax
	mov     r8, rdi
.parse_loop:
	mov     dl, [r8]
	cmp     dl, 0
	je      .parse_done
	cmp     dl, '0'
	jb      .parse_error
	cmp     dl, '9'
	jbe     .digit_num
	jmp     .parse_error
.digit_num:
	sub     dl, '0'
	movzx   ecx, dl
	cmp     ecx, esi
	jae     .parse_error
	xor     rdx, rdx
	mul     rsi
	add     rax, rcx
	inc     r8
	jmp     .parse_loop
.parse_done:
	ret
.parse_error:
	mov     rax, SYS_WRITE
	mov     rdi, SYSOUT
	lea     rsi, [invalid_digit_msg]
	mov     rdx, invalid_digit_msg_len
	syscall
	jmp     exit_program
```


### 계산 및 연산 결과 출력
--- 
구현하고자 하는 프로그램은 이용자로부터 입력받은 정수를 1부터 9까지 차례대로 곱하고, 각 결과를 출력해야 한다. 따라서 곱셈 연산과 출력이 반복되므로, 반복문을 사용해 구현할 수 있다. 

먼저, 정수로 변환한 `input_num`을 `al`에 저장하고, 반복 횟수를 나타내는 루프 카운터를 `r14`에 할당한다. 반복문 내부에서는 루프 카운터인 `r14`와 `al`을 곱하고, 포맷 스트링, `input_num`, `r14`, `al`의 값을 각각의 레지스터에 배치한 뒤 `printf()`를 호출한다. 

반복이 한 사이클 끝날 때마다 `r14`를 1씩 증가시키고, 총 9번의 반복 후 `r14`의 값이 10이 되면 프로그램을 종료한다. 

```nasm
mul_loop:
	cmp     r14, 10 ; r14의 값이 10이 되면 프로그램을 종료
	jmp     exit_program
	xor     rax, rax
	mov     al, byte [input_num]
	imul    rax, r14 ; rax = input_num * r14
	xor     rsi, rsi
	mov     rdi, fmt_mul
	mov     sil, byte [input_num]
	mov     rdx, r14
	mov     rcx, rax
	call    printf ; printf("%d * %d = %d\n", input_num, r14, input_num * r14)
	inc     r14
	jmp     mul_loop
```


### `.text` 섹션 정의
---
C 코드가 `main` 함수부터 실행되듯이, *어셈블리 코드는 운영체제가 인식하는 진입점인 `start`부터 시작*한다. 이는 실행 가능한 코드가 위치하는 `.text` 섹션에서 `global` 키워드와 함께 아래와 같이 정의할 수 있다. 이때 `extern` 키워드를 같이 사용해 외부에 정의된 변수나 함수를 포함할 수도 있다. 

```nasm
section .text
	global _start
	extern printf

_start:
	...
```

>[!example] `global`, `extern`은 무엇일까?
>- `global`: 변수나 함수를 외부에 노출시키는 키워드로, `_start`를 `global`로 선언하면 운영체제가 이를 프로그램의 진입점으로 인식해 이곳부터 프로그램을 실행한다. 
>- `extern`: 해당 변수나 함수가 외부에 정의되어 있다는 것을 명시하는 키워드로, 해당 키워드를 사용하면 외부 코드에 정의된 변수나 함수를 사용할 수 있다. 


### `_start` 작성
---
이 강의에서 작성하는 프로그램은 사용자 입력을 받아 유효성을 검증한 뒤, 반복문으로 계산 결과를 출력하는 흐름으로 동작한다. 이미 핵심적인 함수는 전부 구현했으니, 이제 전체 실행 흐름을 총괄하는 `_start` 루틴을 작성해 보자. 본격적으로 코드를 살펴보기에 앞서, `_start`의 전체 구조는 다음과 같다. 

```nasm
_start: 
	; write(1, prompt_msg, prompt_msg_len)을 system call로 호출
	mov rax, SYS_WRITE
	mov rdi, STDOUT
	mov rsi, prompt_msg
	mov rdx, prompt_msg_len
	syscall
	
	; read(0, input_str, MAX_LEN)
	mov rax, SYS_READ
	mov rdi, STDIN
	lea rsi, [input_str]
	mov rdx, MAX_LEN
	syscall
	cmp rax, 0
	jle exit_program
	
	; 사용자의 입력에서 개행 문자 제거
	lea rdi, [input_str]
	call strip_eol
	
	; parse_number()를 호출해 입력받은 문자열을 정수로 변환
	lea rdi, [input_str]
	mov rsi, 10
	call parse_number
	
	; 예외 처리 - invalid_input()으로 분기
	cmp rax, 0
	je invalid_input
	cmp rax, 100
	jge invalid_input
	mov byte [input_num], al
	
	mov r14, 1
	
mul_loop:
	...
```

`invalid_input()`은 아래에서 구현한다. 


### 정수 범위 검증
---
`invalid_input()`은 이용자의 입력이 0 이하거나 100 이상인 경우, 에러 메시지를 출력하고 프로그램을 종료한다. 내부적으로 `write(1, invalid_msg, invalid_msg_len)`과 동일한 기능이다. 

```nasm
invalid_input:
	mov rax, SYS_WRITE
	mov rdi, STDOUT
	mov rsi, invalid_msg
	mov rdx, invalid_msg_len
	syscall
	jmp exit_program
```


### 프로그램 종료
---
`exit_program()`은 종료 코드 0과 함께 프로그램을 종료한다. 내부적으로는 `exit(0)`과 같다. 

```nasm
exit_program:
	mov rax, SYS_EXIT
	xor rdi, rdi
	syscall
```


### 최종 코드
---
지금까지 구현한 모든 구구단 관련 코드를 통합한 코드는 아래와 같다. 

```nasm
section .data
	prompt_msg:      db "Enter a number (1~99): ", 0
	prompt_msg_len:  equ $ - prompt_msg
	
	invalid_msg:     db "Invalid number! (1 <= number <= 99)", 10, 0
	invalid_msg_len: equ $ - invalid_msg
	
	invalid_digit_msg: db "Error: Invalid digit in input for given base", 10, 0
	invalid_digit_msg_len: equ $ - invalid_digit_msg - 1
	
	fmt_mul: db "%d * %d = %d", 10, 0
	
	STDIN:      equ 0
	STDOUT:     equ 1
	SYS_READ:   equ 0
	SYS_WRITE:  equ 1
	SYS_EXIT:   equ 60
	
	MAX_LEN:    equ 3
	EOL_CHAR:   equ 10
	
section .bss
	input_str:  resb MAX_LEN
	input_num:  resb 1
	
section .text
	global _start
	extern printf
	
_start:
	; 1. 안내 메시지 출력
	mov rax, SYS_WRITE
	mov rdi, STDOUT
	mov rsi, prompt_msg
	mov rdx, prompt_msg_len
	syscall
	
	; 2. 문자열 입력 처리
	mov rax, SYS_READ
	mov rdi, STDIN
	lea rsi, [input_str]
	mov rdx, MAX_LEN
	syscall
	cmp rax, 0
	jle exit_program
	
	; 3. 개행 문자 제거
	lea rdi, [input_str]
	call strip_eol
	
	; 4. 문자열을 정수로 변환
	lea rdi, [input_str]
	mov rsi, 10
	call parse_number
	
	; 5. 정수 범위 비교
	cmp rax, 0
	je invalid_input
	cmp rax, 1000
	jge invalid_input
	mov byte [input_num], al
	
	; 6. 곱셈 및 결과 출력
	mov r14, 1


mul_loop:
	cmp r14, 0
	je exit_program
	xor rax, rax
	mov al, byte [input_num]
	imul rax, r14
	xor rsi, rsi
	mov rdi, fmt_mul
	mov sil, byte [input_num]
	mov rdx, r14
	mov rcx, rax
	call printf
	inc r14
	jmp mul_loop


strip_eol:
	xor rcx, rcx
.strip_loop:
	cmp rcx, MAX_LEN
	jge .done_strip
	cmp byte [rdi+rcx], EOL_CHAR
	je .replace_eol
	inc rcx
	jmp .strip_loop
.replace_eol:
	mov byte [rdi+rcx], 0
.done_strip:
	ret


parse_number:
	xor rax, rax
	mov r8, rdi
.parse_loop:
	mov dl, [r8]
	cmp dl, 0
	je .parse_done
	cmp dl, '0'
	jb .parse_error
	cmp dl, '9'
	jbe .digit_num
	jmp .parse_error
.digit_num:
	sub dl, '0'
	movzx ecx, dl
	cmp ecx, esi
	jae .parse_error
	xor rdx, rdx
	mul rsi
	add rax, rcx
	inc r8
	jmp .parse_loop
.parse_done:
	ret
.parse_error:
	mov rax, SYS_WRITE
	mov rdi, STDOUT
	mov rsi, [invalid_digit_msg]
	mov rdx, invalid_digit_msg_len
	syscall
	jmp exit_program


invalid_input: 
	mov rax, SYS_WRITE
	mov rdi, STDOUT
	mov rsi, invalid_msg
	mov rdx, invalid_msg_len
	syscall
	jmp exit_program


exit_program:
	mov rax, SYS_EXIT
	xor rdi, rdi
	syscall
```


### 컴파일 및 실행
---
작성한 코드를 `gugudan.asm`으로 저장한 후, **아래 명령어**를 *build.sh* 파일로 만들어 실행하면 실행 파일을 생성할 수 있다. 이때, `printf()`와 같은 외부 심볼을 실행 파일에 포함하려면 런타임 로더를 지정하는 `-dynamic-linker /lib64/ld-linux-x86-64.so.2` 옵션과 표준 C 라이브러리를 바이너리에 링크하는 `-lc` 옵션을 함께 지정해야 한다. 

```shell
#!/bin/bash
nasm -f elf64 gugudan.asm -o gugudan.o
ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o gugudan gugudan.o -lc
```

올바르게 코드를 작성했다면, 생성된 실행 파일은 아래 사진처럼 실행되어야 한다. 이때 범위 외의 숫자를 입력한 경우, 두 번째 실행과 같이 정상적으로 에러 메시지를 출력하고 종료된다. 

![[Pasted image 20251219225427.png]]
