### 들어가며
---
이번 강의에서는 이용자로부터 입력받은 숫자를 2진수, 8진수, 10진수, 16진수 표기로 변환하는 **진법 변환기** 프로그램을 어셈블리어로 작성한다. 이용자가 입력한 특정 진법 문자열을 정수 데이터로 변환하는 알고리즘과, 나머지 연산 및 역순 문자열 조합 등을 통해 정수 데이터를 원하는 진법의 문자열로 변환하는 알고리즘을 구현해 본다. 

이전 강의인 [[Assembly Traning - 구구단 구현하기|구구단 구현하기]] 강의와 유사한 부분이 존재하니, 이전 강의를 먼저 본 후에 본 강의를 보고 실습을 진행하는 것을 권장한다. 


### 프로그램 기획
---
진법 변환기는 특정 진법의 형태로 받은 숫자 문자열을 파싱해 원하는 진법 표기로 출력하는 프로그램이다. 본 강의에서는 이용자에게 입력받은 수를 2진법, 8진법, 10진법, 16진법 중 원하는 표기법으로 변환해 주는 기능을 구현한다. 진법 변환기 프로그램은 아래와 같은 기능을 수행해야 한다.

**1. 이용자 입력 처리**

- 진법 및 문자열 입력: 이용자가 입력할 숫자의 진법과 숫자 형태의 문자열을 입력받는다. 
- 문자열의 정수 변환: 이용자에게 입력받은 숫자 형태의 문자열을 정수 데이터로 변환한다.
- 예외 처리: 진법 입력 시 2, 8, 10, 16 이외 다른 값이 입력되거나, 각 진법에 맞지 않은 형태의 문자열을 입력하면 프로그램을 종료한다. 

**2. 정수를 특정 진법 문자열로 변환**

- 정수의 문자열 변환: 이용자가 선택한 진법 표기법에 맞춰 정수를 문자열로 변환

본 강의에서 구현하는 프로그램은 아래와 같이 동작한다. 
![[Pasted image 20251220183743.png]]

1. 프로그램이 시작하면 `Enter source base (2, 8, 10, 16): `이라는 메시지를 출력하고, 사용자에게 2, 8. 10, 16진법 중 하나를 입력받는다. 
2. 이후,  `Enter number in that base: `라는 메시지를 출력하여, 선택한 진법에 맞는 숫자를 입력받는다. 
3. 이어서 `Enter target base (2, 8, 10, 16): `이라는 메시지를 출력하고, 입력받은 숫자를 어떤 진법으로 변환할지 선택하도록 한다. 
4. 마지막으로, 입력된 숫자를 사용자가 선택한 진법으로 변환하여 출력하고 종료한다. 

또한, 아래 사진과 같이 사용자가 프로그램이 지원하지 않는 진법을 선택하거나, 선택한 진법에 맞지 않는 문자열을 입력한다면 에러 메시지를 출력하고 프로그램을 종료하는 방식으로 예외 처리를 구현한다. 

![[Pasted image 20251220185629.png]]

구현에 들어가기 전, 앞서 살펴본 기능들에 필요한 세부 구현을 살펴보자. 구구단 출력 프로그램에서 다뤘던 내용들은 제외하고 살펴본다. 

**1. 특정 진법 문자열의 정수 변환**
사용자에게 2, 8, 10, 16진법으로 문자열을 입력받으면, 이를 정수로 변환해야 한다. 예를 들어, 10진법으로 표기한 1234와 16진법으로 표기한 1234는 표기 자체는 같지만 서로 다른 값을 의미한다. 따라서 각 표기법에 맞게 정수를 올바르게 변환해야 한다. 이는 이전 강의인 **구구단 구현하기**에서 살펴봤듯, **호너의 법칙**을 활용해 변환할 수 있다.

10진법으로 표기한 1234와 16진법으로 표기한 1234를 각각 전개식과 호너의 법칙으로 표현하면 

$$1234_{(10)} = 1 \times 10^3 + 2 \times 10^2 + 3 \times 10^1 + 4 \times 10^0 = ((1 \times 10 + 2)\times 10 + 3)\times 10 + 4$$
$$1234_{(16)} = 1 \times 16^3 + 2 \times 16^2 + 3 \times 16^1 + 4 \times 16^0 = ((1 \times 16 + 2)\times 16 + 3)\times 16 + 4 = 4660_{(10)}$$

**2. 진법 표기 변환과 나머지 연산**
사용자가 입력한 문자열을 정수로 변환한 뒤, 특정 진법으로 변환한 결과를 출력하려면 해당 정수를 다시 원하는 진법에 맞는 문자열로 변환해야 한다. 이는 **위의 과정의 반대 연산**으로, **재귀적인 나눗셈과 나머지 연산**을 통해 구현할 수 있다. 아래의 수식은 10진수 4660을 16진수로 변환하기 위해 재귀적으로 나눗셈과 나머지 연산을 수행하는 예시이다. 

$$4660 \div 16 = 291...4,\ \ 291 \div 16 = 18 ... 3,\ \ 18 \div 16 = 1...2,\ \ 1 \div 16 = 0 ..1$$

위의 나눗셈 식을 하나의 식으로 정리하면 아래와 같이 호너의 법칙으로 나타낸 식을 얻을 수 있으며, 10진수를 16진수로 변환할 수 있다. 

$$4660 =  291 \times 16 + 4 = (18 \times 16 + 3) \times 16 + 4 = ((1 \times 16 + 2) \times 16 + 3) \times 16 + 4 = 1234_{(16)}$$

**3. n진법 문자열 생성**
2번 과정과 같이 나머지 연산을 이용해 진법을 변환하면 가장 하위 자리수 = 문자열의 끝부터 차례대로 변환되기 때문에, 결과 문자열 또한 역순으로 생성된다. 따라서, 올바르게 결과물을 출력하기 위해서는 버퍼의 끝부분부터 문자를 채워야 한다. 즉, <u>문자열 포인터를 버퍼의 맨 마지막 위치로 이동한 뒤, 뒤에서부터 차례대로 각 자릿수에 해당하는 문자를 기록</u>하면 올바른 순서의 진법 문자열을 완성할 수 있다. 

우리가 구현할 진법 변환기는 <u>64비트 레지스터를 하나만 사용하기에 64비트 정수 범위만을 처리할 수 있으며,</u> 그 이상의 범위는 오버플로우로 인한 잘림(Truncation)이 발생하지만 고려하지 않도록 한다. 아래에서는 구현된 코드를 설명하므로, 만약 먼저 구현해 보고 싶다면 충분히 구현해 보고 넘어가는 것을 추천한다. 


### `.data/.bss` 섹션 정의
---
프로그램의 기능을 구현하기 전, 입출력에 사용될 문자열 데이터와 상수를 정의해야 한다. 이전의 강의와 마찬가지로, 본 강의에서는 실습의 편의를 위해 문자열 데이터와 상수 모두 `.data` 섹션에 정의하도록 한다. 

```nasm
section .data
	prompt_src_base:    db "Enter source base (2, 8, 10, 16): ", 0
	prompt_src_base_len:    equ $ - prompt_src_base - 1
	
	prompt_src_value:   db "Enter number in that base: ", 0
	prompt_src_value_len:   equ $ - prompt_src_value - 1
	
	prompt_tgt_base:    db "Enter target base (2, 8, 10, 16): ", 0
	prompt_tgt_base_len:    equ $ - prompt_tgt_base - 1
	
	invalid_base_msg:   db "Error: Unsupported base.", 10, 0
	invalid_base_msg_len:   equ $ - invalid_base_msg - 1
	
	invalid_digit_msg:  db "Error: Invalid digit in input for given base.", 10, 0
	invalid_digit_msg_len:  equ $ - invalid_digit_msg - 1
	
	STDIN:     equ 0
	STDOUT:    equ 1
	SYS_READ:  equ 0
	SYS_WRITE: equ 1
	SYS_EXIT:  equ 60
	
	MAX_BASE_LEN:    equ 8
	MAX_VALUE_LEN:   equ 65
	
	FMT_EOL_CHAR:    db 10
	EOL_CHAR:        equ 10
```

- `prompt_src_base`, `prompt_src_base_len`: 입력 진법을 받을 때 출력할 메시지와 그 길이
- `prompt_src_value`, `prompt_src_value`: 입력 진법에 해당하는 정수 문자열을 받을 때 출력할 메시지와 그 길이
- `prompt_tgt_base`, `prompt_tgt_base_len`: 출력 진법을 받을 때 출력할 메시지와 그 길이
- `invalid_base_msg`, `invalid_base_msg_len`: 잘못된 진법을 입력할 시 출력할 메시지와 그 길이
- `invalid_digit_msg`, `invalid_digit_msg_len`: 문자열을 정수로 변환하는 데 실패했을 때 출력할 메시지와 그 길이
- `MAX_BASE_LEN`, `MAX_VALUE_LEN`: 사용자가 입력할 수 있는 진법과 숫자 문자열의 최대 길이
- `FMT_EOL_CHAR`, `EOL_CHAR`: 개행 문자와 개헹 문자의 아스키 코드 값

그 다음, <u>변수로 사용할 데이터는 .bss 섹션에 정의</u>한다. 초기 값이 정해져 있지 않은 변수들은 `.bss`에 할당된다. `data` 섹션과 비슷하게 `section .bss`로 각 변수를 정의할 수 있다. 

```nasm
section .bss
	base_input_buffer:    resb MAX_BASE_LEN
	base_output_buffer:   resb MAX_BASE_LEN
	number_input_buffer:  resb MAX_VALUE_LEN
	number_output_buffer: resb MAX_VALUE_LEN
	source_base:          resd 1
	target_base:          resd 1
	input_base_value:     resq 1
```

- `base_input(output)_buffer`: 입력 또는 출력할 숫자가 어떤 진법인지를 저장하는 버퍼
- `number_input(output)_buffer`: 입력 또는 출력할 숫자를 저장하는 버퍼

>[!example] `resb`, `resd`, `resq`는 어떻게 다른가?
>모두 `bss` 섹션에 <u>초기화되지 않은 변수를 정의할 때 사용하는 키워드</u>로, 마지막의 알파벳에 따라 자료형의 크기가 달라진다. 
>- `resb`(Reserve Byte): 1 바이트
>- `resw`(Reserve Word): 2 바이트
>- `resd`(Reserve Doubleword): 4 바이트
>- `resq`(Reserve Quadword): 8 바이트


### 이용자 입력 검증
---
우리가 작성하는 진법 변환기 프로그램은 2진법, 8진법, 10진법, 16진법 간의 변환만을 구현한다. 따라서 이용자의 입력을 정수로 변환하고 2, 8, 10, 16인지 검증하는 `validate_base()`라는 함수를 만들고, 이 네 가지 진법 이외의 값을 입력하면 에러 메시지를 출력한 뒤 프로그램을 종료하도록 만들 것이다. 

`validate_base()`는 문자열을 담고 있는 버퍼, 진법 값, 변환된 정수를 저장할 변수를 인자로 받는다. 함수가 종료되면, 첫 번째 인자의 문자열이 두 번째 인자로 받은 진법에 맞게 변환되어 세 번째 인자의 변수에 저장된다. 이때, 문자열을 정수로 변환하는 과정에서는 이전의 구구단 구현하기에서 구현한 `parse_number()`를 그대로 사용한다. 

`validate_base()`는 `_start` 루틴 내부에서 아래와 같이 호출될 수 있으며, 사용자로부터 10진수 형태로 입력을 받을 것이기 때문에 두 번째 인자는 10으로 고정이다. 

```nasm
_start:
    ...
    lea     rdi, [base_input_buffer]
    mov     esi, 10
    lea     rdx, [source_base]
    call validate_base
    ...
```

본격적으로 함수를 구현해 보자. `validate_base()`는 사용자의 입력을 정수로 변환하고, 그 값을 비교하는 함수이다. `parse_number()` 호출 시엔 `validate_base()`를 호출할 때 전달받은 `rdi`와 `rsi` 레지스터의 값을 그대로 인자로 전달한다. 함수 호출 후 반환 값은 `rax`에 저장되며, 반환 값을 검사한 뒤 변수에 값을 저장한다. 만약 2, 8, 10, 16이 아닌 값을 입력할 경우, 에러 메시지를 출력하고 프로그램을 종료한다. 

```nasm
validate_base:
	mov     r10, rax
	call    parse_number
	mov     dword[r10], eax
	cmp     eax, 2
	je      .valid
	cmp     eax, 8
	je      .valid
	cmp     eax, 10
	je      .valid
	cmp     eax, 16
	je      .valid
	mov     rax, SYS_WRITE
	mov     rdi, STDIN
	lea     rsi, [invalid_base_msg]
	mov     rdx, invalid_base_msg_len
	syscall
	jmp     .exit_program
.valid:
	ret
```


### `parse_number()` 기능 확장
---
기존의 `parse_number()`는 첫 번째 인자로 변환할 문자열을, 두 번째 인자로 진법 값을 전달받도록 구현되어 있었으나, 실제 구현에서는 호출 시 항상 10을 전달받아 10진수만을 처리했다. 따라서, 기존의 10진수 범위를 처리하는 것에서 더 나아가 <u>16진수까지 커버할 수 있도록</u> 일부 기능을 확장해 보자. 이를 위해 호출 시 사용자가 입력한 진법을 그대로 넘겨받아, 다양한 진법의 문자열을 처리할 수 있도록 수정해 볼 것이다. 즉, <u>두 번째 인자로 해당 문자열의 진법 값을 전달받음으로써 여러 진수의 문자열 입력을 지원하게 할 것이다.</u>

16진수 문자열까지 변환하기 위해서는 '0'에서 '9'까지의 숫자 범위뿐 아니라 'A'부터 'F'까지의 영어 문자 범위까지 전부 처리할 수 있어야 한다. 이를 위해 각 문자의 아스키 코드 값을 검사해 **'A'부터 'F' 또는 'a'부터 'f' 구간에 속하는 경우 각각 'A', 'a'의 아스키 코드 값을 뺀 뒤 10을 더하는 방식으로 값을 계산하도록 할 것이다.**

아래 코드는 기존의 숫자만 처리하던 기능에 알파벳 대소문자를 포함해 16진수 문자열도 변환할 수 있도록 수정, 에러 처리를 포함한 코드이다. 

```nasm
parse_number:
	xor     rax, rax
	mov     r8, rdi
.parse_loop: ; 16진수 범위까지 커버할 수 있도록 확장
	mov     dl, [r8]
	cmp     dl, 0
	je      .parse_done
	cmp     dl, '0'
	jb      .parse_error
	cmp     dl, '9'
	jbe     .digit_num
	cmp     dl, 'A'
	jb      .parse_error
	cmp     dl, 'F'
	jbe     .digit_hex_upper
	cmp     dl, 'a'
	jb      .parse_error
	cmp     dl, 'f'
	jbe     .digit_hex_lower
	jmp     .parse_error
.digit_num:
	sub     dl, '0'
	jmp     .got_digit
.digit_hex_upper: ; 알파벳 대문자 처리
	sub     dl, 'A'
	add     dl, 10
	jmp     .got_digit
.digit_hex_lower: ; 알파벳 소문자 처리
	sub     dl, 'a'
	add     dl, 10
.got_digit:
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
	mov     rdi, STDOUT
	lea     rsi, [invalid_digit_msg]
	mov     rdx, invalid_digit_msg_len
	syscall
	jmp     exit_program
```


### 정수를 특정 진법 문자열로 변환
---
이전 강의에서는 포맷 스트링과 `printf()`를 사용해 정수를 출력했지만, 본 강의에서는 문자열로 직접 변환한 후 출력하기 위해 정수를 진법에 맞는 문자열로 변환해 주는 `convert_value_to_base()` 함수를 직접 구현하도록 한다. 기획 부분에서 살펴봤듯, 이 함수는 재귀적인 나눗셈과 나머지 연산으로 구현할 수 있다. 또한, 가장 하위 자릿수(문자열의 끝) 부분부터 차례대로 문자로 변환하기 때문에, 버퍼의 맨 마지막에는 문자열의 끝을 나타내는 널 바이트를 먼저 삽입하고, 포인터를 그 앞에 둔 뒤 한 자리씩 역순으로 값을 채워 나간다. 

아래 그림은 10진수 4660을 16진수로 변환하는 과정을 나타낸 것이다.

![[Pasted image 20251220233210.png]]

`convert_value_to_base()`는 첫 번째 인자로 변환하고자 하는 정수인 `input_base_value`를, 두 번째 인자로 변환 대상 진법인 `target_base`, 세 번째 인자로 결과를 저장할 버퍼인 `number_output_buffer`를 전달받는다.

가장 먼저, `r8`에 `number_output_buffer`의 주소를 저장한 후 <u>포인터를 문자열의 끝으로 이동</u>한 뒤 `input_base_value`가 0이 아닌지 비교한다. 0인 경우, `number_output_buffer`에 '0'을 저장한 뒤 함수를 종료하며, 0이 아닌 경우 `.conv_loop` 레이블로 분기해 변환을 시작한다. 

```nasm
convert_value_to_base:
	mov     rax, rdi ; rdi, 즉 첫 번째 인자에는 input_base_value가 들어 있음
	mov     r8, rdx
	add     r8, 63
	cmp     rax, 0
	jne     .conv_loop ; 정수의 값이 0이 아니라면 반복문으로 분기
	mov     byte [r8], '0'
	dec     r8
	jmp     .conv_done ; 정수의 값이 0이라면 버퍼에 '0' 저장 후 종료
```

`.conv_loop` 레이블 이하의 코드는 반복적으로 나눗셈과 나머지 연산을 수행하여 정수를 특정 진법의 문자열로 변환한다. 일반적으로 어셈블리어의 `div` 명령어는 몫을 `rax`에, 나머지를 `rdx`에 저장한다. 따라서, `rdx`가 9 이하인 경우 '0'의 아스키 코드 값인 48을, 10 이상인 경우 'A'의 아스키 코드 값인 65에서 10을 뺀 55를 더한 뒤 `r8`에 저장한다. 

위의 과정을 몫이 0이 될 때까지 = `rax`의 값이 0이 될 때까지 반복한 뒤 함수를 종료한다.

```nasm
.conv_loop
	xor     rdx, rdx
	div     rsi
	cmp     rdx, 9
	jbe     .digit_to_ascii
	add     rdx, 55
	jmp     .store_digit
.digit_to_ascii:
	add     rdx, 48
.store_digit:
	mov     [r8], dl
	dec     r8
	cmp     rax, 0
	jne     .conv_loop ; rax가 0이 될 때까지 반복
.conv_done:
	ret
```

최종적으로 구현한 `convert_value_to_base()`는 다음과 같다. 

```nasm
convert_value_to_base:
	mov     rax, rdi
	mov     r8, rdx
	add     r8, 63
	cmp     rax, 0
	jne     .conv_loop
	mov     byte [r8], '0'
	dec     r8
	jmp     .conv_done
.conv_loop:
	xor     rdx, rdx
	div     rsi
	cmp     rdx, 9
	jbe     .digit_to_ascii
	add     rdx, 55
	jmp     .store_digit
.digit_to_ascii:
	add     rdx, 48
.store_ascii:
	mov     [r8], dl
	dec     r8
	cmp     rax, 0
	jne     .conv_loop
.conv_done:
	ret
```


### `_start` 작성
---
이 프로그램은 이용자에게 총 세 번의 입력을 받으며, 가장 먼저 사용자에게 변환 대상 문자열의 진법을 입력받고 이를 검증한다. 대부분의 핵심 기능은 이미 함수로 구현했기 때문에, 전체 실행 흐름을 총괄하는 `_start` 루틴을 작성해 보도록 한다.

본격적으로 코드를 살펴보기에 앞서, `_start`의 전체 구조는 다음 코드와 같다. 

```nasm
_start:
	; 1. 진법 입력 안내 메시지 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [prompt_src_base]
	mov     rdx, prompt_src_base_len
	syscall
	
	; 2. 진법 입력 처리
	mov     rax, SYS_READ
	mov     rdi, STDIN
	lea     rsi, [base_input_buffer]
	mov     rdx, MAX_BASE_LEN
	syscall
	cmp     rax, 0
	jle     exit_program
	
	; 3. 개행 제거
	mov     rbx, base_input_buffer
	add     rbx, rax
	mov     byte [rbx], 0
	lea     rdi, [base_input_buffer]
	mov     rsi, MAX_BASE_LEN
	call    strip_eol
	
	; 4. 사용자 입력 진법 검증
	lea     rdi, [base_input_buffer]
	mov     esi, 10
	lea     rdx, [source_base]
	call    validate_base
	
	; 5. 문자열 입력 안내 메시지 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [prompt_src_value]
	mov     rdx, prompt_src_value_len
	syscall
	
	; 6. 문자열 입력 처리
	mov     rax, SYS_READ
	mov     rdi, STDIN
	lea     rsi, [number_input_buffer]
	mov     rdx, MAX_VALUE_LEN
	syscall
	cmp     rax, 0
	jle     exit_program
	
	; 7. 개행 제거
	mov     rbx, number_input_buffer
	add     rbx, rax
	mov     byte [rbx], 0
	lea     rdi, [number_input_buffer]
	mov     rsi, MAX_VALUE_LEN
	call    strip_eol
	
	; 8. 문자열을 정수로 파싱
	lea     rdi, [number_input_buffer]
	mov     esi, [source_base]
	call    parse_number
	mov     [input_base_value], rax
	
	; 9. 변환하고자 하는 진법 입력 안내 메시지 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [prompt_tgt_base]
	mov     rdx, prompt_tgt_base_len
	syscall
	
	; 10. 진법 입력 처리
	mov     rax, SYS_READ
	mov     rdi, STDIN
	lea     rsi, [base_output_buffer]
	mov     rdx, MAX_BASE_LEN
	syscall
	cmp     rax, 0
	jle     exit_program
	
	; 11. 개행 처리
	mov     rbx, base_output_buffer
	add     rbx, rax
	mov     byte [rbx], 0
	lea     rdi, [base_output_buffer]
	mov     rsi, MAX_BASE_LEN
	call    strip_eol
	
	; 12. 사용자 입력 진법 검증
	lea     rdi, [base_output_buffer]
	mov     esi, 10
	lea     rdx, [target_base]
	call    validate_base
	
	; 13. 정수를 문자열로 파싱
	mov     rdi, [input_base_value]
	mov     esi, [target_base]
	lea     rdx, [number_output_buffer]
	call    convert_value_to_base
	
	; 14. 결과 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [number_output_buffer]
	mov     rdx, MAX_VALUE_LEN
	syscall
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [FMT_EOL_CHAR]
	mov     rdx, 1
	syscall
	jmp     exit_program
```

우선, `read` 시스템 콜과 `write` 시스템 콜을 사용해 안내 메시지를 출력하고 사용자에게 데이터를 입력받은 뒤, `strip_eol()`을 호출해 개행 문자를 제거한 뒤 `validate_base()`를 호출해 사용자의 입력이 적절한지 검증한다. 이 과정은 이후 사용자가 원하는 진법을 입력할 때도 동일하게 적용되며, 중복되는 서술이므로 생략한다. 

마찬가지로 `read` 시스템 콜과 `write` 시스템 콜을 사용해 안내 메시지를 출력하고 사용자에게 데이터를 입력받은 뒤, 개행 문자를 제거한다. 이후, `parse_number()`를 출력해 사용자가 입력한 문자열을 정수로 적절하게 변환한다. 

사용자에게 원하는 변환 대상 진법을 입력받은 뒤, `convert_value_to_base()`를 호출해 사용자가 입력한 정수를 특정 진법의 표기에 맞는 문자열로 변환한다. 변환 결과는 `number_output_buffer`에 저장되며, `write` 시스템 콜을 호출해 출력한 뒤 프로그램을 호출한다. 

최종적으로 작성된 `_start` 루틴은 다음과 같다. 
```nasm
_start:
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [prompt_src_base]
    mov     rdx, prompt_src_base_len
    syscall
    
    mov     rax, SYS_READ
    mov     rdi, STDIN
    lea     rsi, [base_input_buffer]
    mov     rdx, MAX_BASE_LEN
    syscall
    cmp     rax, 0
    jle     exit_program
    
    mov     rbx, base_input_buffer
    add     rbx, rax
    mov     byte [rbx], 0
    lea     rdi, [base_input_buffer]
    mov     rsi, MAX_BASE_LEN
    call    strip_eol
    
    lea     rdi, [base_input_buffer]
    mov     esi, 10
    lea     rdx, [source_base]
    call    validate_base
    
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [prompt_src_value]
    mov     rdx, prompt_src_value_len
    syscall
    
    mov     rax, SYS_READ
    mov     rdi, STDIN
    lea     rsi, [number_input_buffer]
    mov     rdx, MAX_VALUE_LEN
    syscall
    cmp     rax, 0
    jle     exit_program
    
    mov     rbx, number_input_buffer
    add     rbx, rax
    mov     byte [rbx], 0
    lea     rdi, [number_input_buffer]
    mov     rsi, MAX_VALUE_LEN
    call    strip_eol
    
    lea     rdi, [number_input_buffer]
    mov     esi, [source_base]
    call    parse_number
    mov     [input_base_value], rax
    
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [prompt_tgt_base]
    mov     rdx, prompt_tgt_base_len
    syscall
    
    mov     rax, SYS_READ
    mov     rdi, STDIN
    lea     rsi, [base_output_buffer]
    mov     rdx, MAX_BASE_LEN
    syscall
    cmp     rax, 0
    jle     exit_program
    
    mov     rbx, base_output_buffer
    add     rbx, rax
    mov     byte [rbx], 0
    lea     rdi, [base_output_buffer]
    mov     rsi, MAX_BASE_LEN
    call    strip_eol
    
    lea     rdi, [base_output_buffer]
    mov     esi, 10
    lea     rdx, [target_base]
    call validate_base
    
    mov     rdi, [input_base_value]
    mov     esi, [target_base]
    lea     rdx, [number_output_buffer]
    call    convert_value_to_base
    
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [number_output_buffer]
    mov     rdx, MAX_VALUE_LEN
    syscall
    
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [FMT_EOL_CHAR]
    mov     rdx, 1
    syscall
    jmp exit_program
```


### 최종 코드
---
앞서 구현한 코드와 이전 강의의 코드를 기반으로 최종 프로그램을 작성하면 다음과 같다. 

```nasm
section .data
	prompt_src_base:     db "Enter source base (2, 8, 10, 16): ", 0
	prompt_src_base_len:     equ $ - prompt_src_base - 1
	
	prompt_src_value:     db "Enter number in that base: ", 0
	prompt_src_value_len:     equ $ - prompt_src_value - 1
	
	prompt_tgt_base:     db "Enter target base (2, 8, 10, 16): ", 0
	prompt_tgt_base_len:     equ $ - prompt_tgt_base - 1
	
	invalid_base_msg:     db "Error: Unsupported base.", 10, 0
	invalid_base_msg_len:     equ $ - invalid_base_msg - 1
	
	invalid_digit_msg:     db "Error: Invalid digit in input for given base.", 10, 0
	invalid_digit_msg_len:     equ $ - invalid_digit_msg - 1
	
	STDIN:     equ 0
	STDOUT:     equ 1
	SYS_READ:      equ 0
	SYS_WRITE:      equ 1
	SYS_EXIT:    equ 60
	
	MAX_BASE_LEN:     equ 8
	MAX_VALUE_LEN:     equ 65
	
	FMT_EOL_CHAR:     db 10
	EOL_CHAR:     equ 10
	
section .bss
	base_input_buffer:     resb MAX_BASE_LEN
	base_output_buffer:     resb MAX_BASE_LEN
	number_input_buffer:     resb MAX_VALUE_LEN
	number_output_buffer:     resb MAX_VALUE_LEN
	source_base:     resd 1
	target_base:     resd 1
	input_base_value:      resq 1
	
section .text
	global _start
	
_start:
	; 1. 진법 입력 안내 메시지 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [prompt_src_base]
	mov     rdx, prompt_src_base_len
	syscall
	
	; 2. 진법 입력 처리
	mov     rax, SYS_READ
	mov     rdi, STDIN
	lea     rsi, [base_input_buffer]
	mov     rdx, MAX_BASE_LEN
	syscall
	cmp     rax, 0
	jle     exit_program
	
	; 3. 개행 제거
	mov     rbx, base_input_buffer
	add     rbx, rax
	mov     byte [rbx], 0
	lea     rdi, [base_input_buffer]
	mov     rsi, MAX_BASE_LEN
	call    strip_eol
	
	; 4. 사용자 입력 진법 검증
	lea     rdi, [base_input_buffer]
	mov     esi, 10
	lea     rdx, [source_base]
	call    validate_base
	
	; 5. 문자열 입력 안내 메시지 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [prompt_src_value]
	mov     rdx, prompt_src_value_len
	syscall
	
	; 6. 문자열 입력 처리
	mov     rax, SYS_READ
	mov     rdi, STDIN
	lea     rsi, [number_input_buffer]
	mov     rdx, MAX_VALUE_LEN
	syscall
	cmp     rax, 0
	jle     exit_program
	
	; 7. 개행 제거
	mov     rbx, number_input_buffer
	add     rbx, rax
	mov     byte [rbx], 0
	lea     rdi, [number_input_buffer]
	mov     rsi, MAX_VALUE_LEN
	call    strip_eol
	
	; 8. 문자열을 정수로 파싱
	lea     rdi, [number_input_buffer]
	mov     esi, [source_base]
	call    parse_number
	mov     [input_base_value], rax
	
	; 9. 변환하고자 하는 진법 입력 안내 메시지 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [prompt_tgt_base]
	mov     rdx, prompt_tgt_base_len
	syscall
	
	; 10. 진법 입력 처리
	mov     rax, SYS_READ
	mov     rdi, STDIN
	lea     rsi, [base_output_buffer]
	mov     rdx, MAX_BASE_LEN
	syscall
	cmp     rax, 0
	jle     exit_program
	
	; 11. 개행 처리
	mov     rbx, base_output_buffer
	add     rbx, rax
	mov     byte [rbx], 0
	lea     rdi, [base_output_buffer]
	mov     rsi, MAX_BASE_LEN
	call    strip_eol
	
	; 12. 사용자 입력 진법 검증
	lea     rdi, [base_output_buffer]
	mov     esi, 10
	lea     rdx, [target_base]
	call    validate_base
	
	; 13. 정수를 문자열로 파싱
	mov     rdi, [input_base_value]
	mov     esi, [target_base]
	lea     rdx, [number_output_buffer]
	call    convert_value_to_base
	
	; 14. 결과 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [number_output_buffer]
	mov     rdx, MAX_VALUE_LEN
	syscall
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [FMT_EOL_CHAR]
	mov     rdx, 1
	syscall
	jmp     exit_program


strip_eol:
	xor     rcx, rcx
.strip_loop:
	cmp     rcx, rsi
	jge     .done_strip
	cmp     byte [rdi+rcx], EOL_CHAR
	je      .replace_eol
	inc     rcx
	jmp     .strip_loop
.replace_eol:
	mov     byte [rdi+rcx], 0
.done_strip:
	ret
	
	
parse_number:
	xor     rax, rax
	mov     r8, rdi
.parse_loop: ; 16진수 범위까지 커버할 수 있도록 확장
	mov     dl, [r8]
	cmp     dl, 0
	je      .parse_done
	cmp     dl, '0'
	jb      .parse_error
	cmp     dl, '9'
	jbe     .digit_num
	cmp     dl, 'A'
	jb      .parse_error
	cmp     dl, 'F'
	jbe     .digit_hex_upper
	cmp     dl, 'a'
	jb      .parse_error
	cmp     dl, 'f'
	jbe     .digit_hex_lower
	jmp     .parse_error
.digit_num:
	sub     dl, '0'
	jmp     .got_digit
.digit_hex_upper: ; 알파벳 대문자 처리
	sub     dl, 'A'
	add     dl, 10
	jmp     .got_digit
.digit_hex_lower: ; 알파벳 소문자 처리
	sub     dl, 'a'
	add     dl, 10
.got_digit:
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
	mov     rdi, STDOUT
	lea     rsi, [invalid_digit_msg]
	mov     rdx, invalid_digit_msg_len
	syscall
	jmp     exit_program
	
	
validate_base:
	mov     r10, rax
	call    parse_number
	mov     dword[r10], eax
	cmp     eax, 2
	je      .valid
	cmp     eax, 8
	je      .valid
	cmp     eax, 10
	je      .valid
	cmp     eax, 16
	je      .valid
	mov     rax, SYS_WRITE
	mov     rdi, STDIN
	lea     rsi, [invalid_base_msg]
	mov     rdx, invalid_base_msg_len
	syscall
	jmp     .exit_program
.valid:
	ret
	
	
convert_value_to_base:
	mov     rax, rdi
	mov     r8, rdx
	add     r8, 63
	cmp     rax, 0
	jne     .conv_loop
	mov     byte [r8], '0'
	dec     r8
	jmp     .conv_done
.conv_loop:
	xor     rdx, rdx
	div     rsi
	cmp     rdx, 9
	jbe     .digit_to_ascii
	add     rdx, 55
	jmp     .store_digit
.digit_to_ascii:
	add     rdx, 48
.store_ascii:
	mov     [r8], dl
	dec     r8
	cmp     rax, 0
	jne     .conv_loop
.conv_done:
	ret
	
	
exit_program:
	mov     rax, SYS_EXIT
	xor     rdi, rdi
	syscall
```


### 컴파일 및 실행
---
작성한 코드를 `base_translator.asm`으로 저장 후, **아래 명령어**를 *build.sh* 파일로 만들어 실행하면 실행 파일을 생성할 수 있다. 

```shell
#!/bin/bash
nasm -f elf64 base_translator.asm -o base_translator.o
ld base_translator.o -o base_translator
```

올바르게 코드를 작성했다면, 생성된 실행 파일은 아래 사진처럼 실행되어야 한다. 이때 제공되지 않는 진법을 입력하거나, 선택한 진법에 맞지 않는 숫자를 입력할 경우, 두 번째와 세 번째 실행과 같이 에러 메시지를 출력한 뒤 프로그램이 종료된다. 

![[Pasted image 20251221010123.png]]