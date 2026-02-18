### 들어가며
---
이번 강의에서는 Linux의 vi 에디터의 일부 기능을 구현한, 간단한 **텍스트 편집기** 프로그램을 어셈블리어로 작성해 볼 것이다. `open`, `read`, `write` 시스템 콜을 활용해 파일 입출력과 관련된 기능을 구현하고, 에디터를 종료하거나 파일을 저장하는 등의 기능을 수행하기 위한 키보드 입력 처리도 함께 구현해 볼 것이다. 

본 강의에서 제작할 편집기는 파일을 생성하거나 열고, 뒤에 텍스트를 덧붙이는 방식으로만 작동하는 아주 간단한 프로그램이다. 따라서 커서를 이동하는 등의 기능은 포함되어 있지 않으며, 실제 텍스트 편집기 수준의 프로그램을 만들기 위해서는 고려할 점이 많이 있기 때문에 이번 강의에서는 핵심적인 동작인 <u>텍스트 작성과 파일 저장</u>만 다룰 것이다. 


### 프로그램 기획
---
본 강의에서 구현할 텍스트 편집기 프로그램은 명령줄의 인자로 파일 이름을 받아서 해당 파일을 편집하는 형태로 동작한다. vi 에디터와 비슷하게, 파일에 저장된 내용을 출력하거나 새로운 내용을 추가할 수 있으며, ESC 키가 입력될 경우 파일을 저장하거나 프로그램을 종료하는 등 특수한 동작을 수행한다. 

텍스트 편집기 프로그램은 크게 아래의 세 가지 기능을 구현해야 한다.

**1. 파일 열기 및 입출력**

- 파일 열기 및 생성: `open` 시스템 콜을 사용해 파일을 열고, 파일이 없다면 생성한다. 
- 파일 입출력: `read`와 `write` 시스템 콜을 사용해 파일에 작성되어 있던 내용을 읽어 화면에 출력하거나, `close` 시스템 콜로 프로그램 종료와 함께 파일을 저장한다.


**2. 터미널 설정**

- 터미널 모드 전환: 버퍼를 이용해 줄 단위로 입력과 출력을 처리하는 정규 모드 대신, 문자 단위로 처리할 수 있는 비정규 모드를 사용하도록 터미널 설정을 변경한다. 


**3. 키보드 입력 처리**

- 문자 단위 입출력: 줄 단위가 아닌 문자 단위로 입력과 출력을 처리한다. 
- 프로그램 종료 및 저장: ESC 키를 입력한 후, 특정 명령어를 입력해 파일을 저장하거나 프로그램을 종료할 수 있다. 

본 강의에서 구현하는 프로그램의 최종 출력은 아래 사진과 같다. 아래는 본 강의에서 구현한 텍스트 편집기를 사용해 "hello dreamhack" 문자열을 *dreamhack.txt*에 저장하고 출력한다. 편집기 구현을 위해서는 사전에 알아야 할 배경 지식이 있기 때문에, 하나씩 살펴보자. 

![[Pasted image 20251225033707.png]]

>[!example] 터미널의 정규(Canonical) 모드와 비정규(Non-Canonical) 모드
>**정규 모드(Canonical Mode)** 는 터미널을 사용하면 기본적으로 설정되는 모드로, <u>개행 문자, EOF(End OF File), EOL(End Of Line)</u>으로 이용자의 입력을 구분하여 처리한다. 즉, 일반적인 상황에서는 사용자가 엔터 키를 입력하기 전까지는 문자열이 전달되지 않으며, 엔터 키를 누르는 순간 명령어가 실행되는 이유는 터미널이 정규 모드 상태이기 때문이다. 
>
>**비정규 모드(Non-Canonical Mode)** 는 줄 단위로 입력을 처리하지 않고, <u>바이트 단위로 입력을 처리한다.</u> 비정규 모드의 예시로 Linux의 vi 에디터를 떠올릴 수 있다. vi 에디터의 입력 모드에서는 개행 문자를 입력해도 프로그램이 종료되거나 명령어로 인식해 실행되지 않으며, 모든 입출력이 문자 단위로 이루어진다. 사용자의 입력이 바로 처리되고 터미널에 출력되는 이유도 터미널이 비정규 모드 상태이기 때문이다. 


### 시스템 콜
---
편집기 프로그램을 구현하기 위해서는 다양한 시스템 콜이 사용된다. 아래의 표는 프로그램 구현에 사용할 시스템 콜을 정리해 놓은 것이다. 

| 시스템 콜   | 번호  | 용도                        |
| ------- | --- | ------------------------- |
| `read`  | 0   | 파일이나 표준 입력으로부터 데이터 읽기에 사용 |
| `write` | 1   | 화면 출력 및 파일에 데이터를 쓰기에 사용   |
| `open`  | 2   | 파일 열기 및 생성에 사용            |
| `close` | 3   | 파일 디스크립터를 닫을 때 사용         |
| `lseek` | 8   | 파일 오프셋 이동에 사용             |
| `ioctl` | 16  | 입출력 속성 제어에 사용             |
| `exit`  | 60  | 프로세스 종료에 사용               |

이 중 이전 강의에서는 언급하지 않았던 `open`, `close`, `lseek`, `ioctl` 시스템 콜은 

- `open`: 파일 열기에 성공했을 경우 파일 디스크립터 값을 반환, 실패할 경우 -1을 반환
- `close`: 파일 디스크립터를 인자로 받아 파일을 닫을 때 사용
- `lseek`: 첫 번째 인자로 파일 디스크립터를, 두 번째 인자로 오프셋을, 세 번째 인자로 옵션을 받아 오프셋과 옵션에 따라 파일 포인터의 위치를 변경
- `ioctl`: 입출력 제어(I/O Control)의 약자로, `read`나 `write` 같은 일반적인 입출력으로 처리할 수 없는, 장치 드라이버와 이용자 공간 프로그램 간의 특수한 입출력을 제어할 때 사용, 본 프로그램에서는 터미널 설정 변경을 위해 사용


### `termios` 구조체
---
`termios`는 **POSIX 계열 운영체제**에서 터미널의 동작 모드를 제어하기 위해 사용하는 구조체로, C 언어에서 `tcgetattr()`을 통해 현재 `termios` 구조체의 값을 가져올 수 있고, `tcsetattr()`을 통해 `termios` 구조체의 값을 수정할 수 있다. 하지만 어셈블리 단계에서는 구조체를 제어하기 위해 `ioctl` 시스템 콜을 대신 사용해야 한다. 

`termios` 구조체는 아래와 같이 선언되어 있으며, 이 중 `c_lflag`와 `c_cc`는 이후에 편집기를 구현할 때 사용한다. 

```cpp
struct termios {
  tcflag_t c_iflag;                /* input mode flags */
  tcflag_t c_oflag;                /* output mode flags */
  tcflag_t c_cflag;                /* control mode flags */
  tcflag_t c_lflag;                /* local mode flags */
  cc_t c_line;                        /* line discipline */
  cc_t c_cc[NCCS];                /* control characters */
  ...
};
```

- `c_iflag`: 입력이 프로그램에 전달되기 전의 처리 방식 제어
- `c_oflag`: 프로그램에서 받은 문자가 화면에 출력되기 전의 처리 방식 제어
- `c_cflag`: 물리적 장치와 통신할 때 데이터 크기와 전송 속도 등을 제어
- `c_lflag`: 터미널의 특성을 제어, 정규 모드와 비정규 모드를 전환 가능
- `c_cc[NCCS]`: 특수 제어 문자의 코드 값을 저장하는 배열로, 본 프로그램 구현 시 사용할 항목은 `VMIN`과 `VTIME`이다. 
	- `VMIN`: 비정규 모드에서 `read` 시스템 콜이 언제 반환될지 결정, 1로 설정하면 문자 하나당 `read` 1개가 반환되고, 5로 설정하면 문자가 5개 입력되기 전까지 블로킹됨
	- `VTIME`: 비정규 모드에서 입력의 최대 대기 시간 설정, 단위는 100ms이며, 0으로 설정하면 최대 대기 시간 없이 `VMIN` 조건이 충족될 때 바로 `read` 반환

본 강의는 터미널이나 `termios` 구조체가 아닌, 다양한 시스템 콜을 사용하여 어셈블리어로 프로그램을 작성하는 데 초점을 둔다. 따라서, 각각의 설정이나 옵션에 대한 자세한 설명은 흐름상 생략한다. 또한, 다음 토픽부터는 이미 구현된 코드를 설명하기 때문에 위의 내용들을 보고 스스로 구현이 가능한 부분까지 생각해 본 뒤 넘어가는 것을 추천한다. 


### `.data` 섹션 정의
---
프로그램의 기능을 구현하기 전, 입출력에 사용할 문자열 데이터와 상수를 정의한다. 이전의 두 강의와 마찬가지로, 본 강의에서는 실습에서의 편의를 위해 문자열 데이터와 상수 모두 `.data` 섹션에 정의한다. 정의되는 데이터가 꽤 많기 때문에, 천천히 살펴보자. 

```nasm
section .data
	usage_msg:     db "Usage: editor <filename>", 10
	usage_msg_len: equ $ - usage_msg
	
	open_err_msg:  db "Error: cannot open file", 10
	open_err_msg_len: equ $ - open_err_msg
	
	SYS_READ     equ 0
	SYS_WRITE    equ 1
	SYS_OPEN     equ 2
	SYS_CLOSE    equ 3
	SYS_LSEEK    equ 8
	SYS_IOCTL    equ 16
	SYS_EXIT     equ 60
	
	STDIN        equ 0
	STDOUT       equ 1
	O_RDWR       equ 2
	O_CREAT      equ 64
	TCGETS       equ 0x5401
	TCSETS       equ 0x5402
	ICANON_FLAG  equ 2
	ECHO_FLAG    equ 8
	
	ENTER_ALT_SCREEN:     db 0x1b, "[?1049h", 0
	ENTER_ALT_SCREEN_LEN: equ $ - ENTER_ALT_SCREEN
	EXIT_ALT_SCREEN:      db 0x1b, "[?1049l", 0
	EXIT_ALT_SCREEN_LEN:  equ $ - EXIT_ALT_SCREEN
	CLEAR_SCREEN:         db 0x1b, "[2J", 0x1b, "[H", 0
	CLEAR_SCREEN_LEN:     equ $ - CLEAR_SCREEN
```

- `usage_msg`, `usage_msg_len`: argc의 값이 2가 아닌 경우 출력할 오류 메시지와 그 길이
- `open_err_msg`, `open_err_msg_len`: 파일 열기에 실패했을 경우 출력할 메시지와 그 길이
- `SYS_*`: 각 시스템 콜의 번호
- `O_RDWR`, `O_CREAT`: `open` 시스템 콜을 호출할 때 사용하는 옵션으로, 읽기/쓰기 권한으로 파일을 열고, 파일이 없으면 새로 생성한다. 
- `TCGETS`, `TCSETS`: `ioctl` 시스템 콜을 통해 터미널에서 정규 모드와 비정규 모드를 설정할 때 사용하는 옵션
- `ICANON_FLAG`, `ECHO_FLAG`: 터미널의 정규 모드와 비정규 모드를 설정하기 위해 필요한 플래그 값
- **ANSI 이스케이프 시퀀스**로 터미널을 제어하는 문자열을 정의
	- `ENTER_ALT_SCREEN`, `ENTER_ALT_SCREEN_LEN`: 터미널의 **대체 화면 버퍼(Alternate Screen Buffer)** 로 진입하는 문자열과 그 길이
	- `EXIT_ALT_SCREEN`, `EXIT_ALT_SCREEN_LEN`: 대체 화면 버퍼에서 원래의 화면 버퍼로 복귀하는 문자열과 그 길이
	- `CLEAR_SCREEN`, `CLEAR_SCREEN_LEN`: 화면을 완전히 지운 후 커서를 첫 줄로 이동시키는 문자열과 그 길이


>[!example] argc란? 
>**argc**는 **Argument Count**의 줄임말로, 명령줄 인수의 개수를 의미한다. 예를 들어, `cat test.txt`라는 명령어를 입력했다면, 매개변수는 `cat`과 `test.txt`로 총 2개이며, argc의 값도 2이다. 

>[!example] ANSI 이스케이프 시퀀스란?
>**ANSI 이스케이프 시퀀스(ANSI Escape Sequence)** 또는 **터미널 제어 시퀀스(Terminal Control Sequence)** 는 터미널의 특수한 효과를 위해 사용하는 문자 조합으로, 터미널의 글자 색 변경, 커서 위치 변경, 화면 전환 및 초기화 등에 사용할 수 있다. 

>[!example] 대체 화면 버퍼란?
>**대체 화면 버퍼(Alternative Screen Buffer)** 는 일부 터미널에서 메인 화면과 별개로 제공하는 버퍼이다. 이 버퍼가 활성화되면, 터미널은 <u>현재의 화면 내용을 숨기고 대체 화면 버퍼를 표시</u>한다. 대체 화면은 텍스트 편집기나 터미널에서 실행되는 게임처럼 전체 터미널 화면을 사용하되, 기존의 터미널 내용을 방해햐지 않고자 할 때 유용하게 사용할 수 있다. 


### `.bss` 섹션 정의
---
그 다음, 변수로 저장할 데이터는 `.bss` 섹션에 정의한다. 

```nasm
section .bss
    buffer:      resb 8192
    buf_len:     resq 1
    termios_old: resb 66
    termios_new: resb 66
    key_buf:     resb 1
```

- `buffer`: 파일 내용을 저장하고, 편집 중인 텍스트를 저장할 버퍼
- `buf_len`: 현재 `buffer`에 저장된 문자 수를 기록
- `termios_old`, `termios_new`: 터미널 설정을 위한 `termios` 구조체를 저장할 변수
- `key_buf`: 키보드에서 읽은 한 글자를 임시로 담아 둘 1 바이트 공간


### 터미널 설정
---
일반적으로 터미널은 사용자의 입력을 줄 단위로 처리하는 정규 모드가 기본 세팅으로 되어 있다. 해당 프로그램은 줄 단위가 아닌 문자 단위로 처리할 것이기 때문에, 터미널 모드를 비정규 모드로 변경해야 한다. 

Linux에서는 `ioctl` 시스템 콜을 통해 터미널의 `termios` 구조체 정보를 가져오거나 설정을 변경할 수 있다. `ioctl(fd, TCGETS, &orig_termios)`처럼 호출해 현재 터미널 설정을 백업해 놓은 뒤, 복사본의 `c_lflag` 필드에서 `ICANON` 비트를 끄면 기존에 줄 단위로 처리되던 라인 버퍼링이 해제되고 즉시 입력 모드가 되며, `ECHO` 비트를 끄면 입력된 문자를 화면에 자동으로 출력하지 않게 되며 비정규 모드로 전환할 수 있다. 

변경된 설정은 `ioctl(fd, TCSETS, &new_termios)`와 같이 호출해서 적용하며, 프로그램이 종료될 때는 백업해 둔 원래의 터미널 설정을 `TCSETS`로 다시 설정해서 터미널을 정규 모드로 복귀시킬 수 있다. 

먼저, 터미널 설정 정보를 백업하고 적용하는 함수를 구현해 보자. 

**`saved_termios()`**
아래의 코드는 `ioctl`을 호출해 `TCGETS` 요청으로 현재 정규 모드인 `termios` 구조체 정보를 `termios_old`에 저장하며, 실패했을 시 `fatal_exit()`을 호출해 프로그램을 종료한다. `fatal_exit(0`은 아래에서 자세하게 다룬다. 

```nasm
saved_termios:
	mov     rax, SYS_IOCTL
	mov     rdi, STDIN
	mov     rsi, TCGETS
	lea     rdx, [termios_old]
	syscall
	test    rax, rax
	js      fatal_exit
	ret
```

**`set_non_canonical_mode()`**
아래의 코드는 기존의 터미널 설정을 `termios_old`에 저장한 뒤, 비정규 모드로 변환하기 위한 일부 설정을 변경한 후 적용하는 함수이다. `TCGETS`로 값을 읽어온 뒤 `termios` 구조체를 수정하며, 변경된 설정을 `TCSETS`로 적용한다. 이 때 `ICANON` 비트와 `ECHO` 비트를 꺼서 비정규 모드로 전환하고, `VTIME`과 `VMIN`을 각각 0과 1로 설정해 <u>시간 지연 없이 문자 단위로 입출력할 수 있도록</u> 설정한다. 

```nasm
set_non_canonical_mode:
	mov     rax, SYS_IOCTL
	mov     rdi, STDIN
	mov     rsi, TCGETS
	lea     rdx, [termios_new]
	syscall
	
	and     dword [termios_new + 12], ~(ICANON_FLAG | ECHO_FLAG)
	mov     byte [termios_new + 22], 0 ; VTIME 설정
	mov     byte [termios_new + 23], 1 ; VMIN 설정
	
	mov     rax, SYS_IOCTL
	mov     rdi, STDIN
	mov     rsi, TCSETS
	lea     rdx, [termios_new]
	syscall
	ret
```


### 화면 처리
---
터미널에는 일반 화면 버퍼와 대체 화면 버퍼가 존재한다. 대체 화면 버퍼는 현재 Shell 화면의 내용을 백업해 두고, 보이지 않던 별도의 빈 화면으로 전환하여 <u>프로그램이 기존 화면을 덮어쓰지 않고 터미널 화면을 사용할 수 있도록</u> 한다. 

Linux의 vi 에디터는 처음 실행 시 화면이 모두 지워지며, 커서가 좌측 상단으로 이동하는데, 이와 비슷한 구현을 위해 **대체 화면 버퍼**를 이용할 수 있다. 화면 제어는 터미널에 **ESC 문자**를 전송하는 방식으로 이뤄지며, 이는 `write` 시스템 콜로 구현할 수 있다. 

`store_terminal()`
아래의 코드는 기존의 Shell 화면을 백업하고, 화면 버퍼를 일반 화면에서 대체 화면으로 전환한다. 이 명령을 실행하면 현재의 Shell 화면 대신, 비어 있는 버퍼가 화면에 출력된다. 

```nasm
store_terminal:
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [ENTER_ALT_SCREEN]
	mov     rdx, ENTER_ALT_SCREEN_LEN
	syscall
	ret
```

`clear_screen()`
아래의 코드는 버퍼를 초기화하고, 커서를 다시 좌측 상단으로 이동시키는 코드이며, 이는 대체 화면 버퍼 출력 시, 이전에 대체 화면을 사용한 프로그램이 있다면 버퍼에 데이터가 남아 있기 때문이다. 

```nasm
clear_screen:
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	lea     rsi, [CLEAR_SCREEN]
	mov     rdx, CLEAR_SCREEN_LEN
	syscall
	ret
```


### 터미널 복귀
---
에디터의 실행이 끝난 후, 변경했던 기존의 터미널 설정을 다시 원래의 정규 모드로 복귀시켜야 한다. 이는 아래의 `restore_terminal()`로 구현할 수 있다. 

```nasm
restore_terminal:
	mov     rax, SYS_IOCTL
	mov     rdi, STDIN
	mov     rsi, TCSETS
	lea     rdx, [termios_old]
	syscall
	test    rax, rax
	js      fatal_exit
	
	mov     rax, SYS_IOCTL
	mov     rdi, STDIN
	lea     rsi, [EXIT_ALT_SCREEN]
	mov     rdx, EXIT_ALT_SCREEN_LEN
	syscall
	ret
```


### 파일 저장
---
사용자가 편집을 모두 마쳤다면, 프로그램을 종료하기 전 작성한 내용을 파일에 저장해야 한다. 이는 `buffer`에 작성한 내용을 파일에 쓰는 것과 동일하며, 이때 `lseek` 호출 없이 `buffer`의 내용을 파일에 저장한다면 <u>기존에 존재하던 내용이 중복해서 작성되기 때문에 파일 포인터의 값을 0으로 초기화해야 한다.</u>

아래 사진은 *test.txt*에 "hello"라는 문자열이 이미 저장되어 있을 때, " dreamhack" 문자열을 덧붙여 저장하는 예시를 보여 준다. 

![[Pasted image 20251225060018.png]]

`lseek`를 호출하지 않는다면 기존의 파일 뒤에 덧붙여져 "hellohello dreamhack"이 저장되지만, `lseek`를 호출해 파일 포인터를 맨 앞으로 이동한다면 의도한 대로 "hello dreamhack"이 파일에 저장된다. 

이 내용을 어셈블리어로 구현한다면 아래와 같이 `save_file()`로 구현할 수 있다. 이때, `r12` 레지스터에는 파일 디스크립터가 저장된다. 

```nasm
save_file:
	mov     rax, SYS_LSEEK
	mov     rdi, r12
	xor     rsi, rsi
	xor     rdx, rdx
	syscall
	
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	mov     rsi, buffer
	mov     rdx, [buf_len]
	syscall
	ret
```


### 예외 처리
---
본 프로그램은 argc의 값이 2가 아닌 경우 또는 파일 열기에 실패했을 경우, 오류 메시지와 함께 프로그램을 종료한다. 

아래의 `no_argument()`는 전달된 매개변수가 너무 적거나 너무 많은 경우 오류 메시지를 출력하고 프로그램을 종료하는 코드이다. 

```nasm
no_argument:
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	mov     rsi, usage_msg
	mov     rdx, usage_msg_len
	syscall
	jmp     fatal_exit
```

아래의 `open_fail()`은 파일 열기에 실패했을 떄 오류 메시지를 출력하고 프로그램을 종료한다. 

```nasm
open_fail:
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	mov     rsi, open_err_msg
	mov     rdx, open_err_msg_len
	syscall
	jmp     fatal_exit
```


### 프로그램 종료
---
`fatal_exit()`은 시스템 콜이 의도대로 동작하지 않은 특수한 경우에 호출하며, **비정상 종료**를 의미하는 종료 코드 1과 함께 프로그램을 종료한다. 

```nasm
fatal_exit:
	mov     rax, SYS_EXIT
	mov     rdi, 1
	syscall
```

`exit_program()`은 `fatal_exit()`와 다르게, 의도대로 프로그램이 동작하고 종료되었을 경우에 호출하며, `restore_terminal()`을 호출해 <u>이전의 정규 모드와 일반 화면 버퍼로 복귀하고, 종료 코드 0과 함께 프로그램을 종료한다.</u>

```nasm
exit_program:
	call    restore_terminal
	mov     rax, SYS_CLOSE
	mov     rdi, r12 ; r12에는 파일 디스크립터가 저장됨
	syscall
	mov     rax, SYS_EXIT
	xor     rdi, rdi ; 종료 코드 0 전달
	syscall
```


### `.start` 작성
---
이 프로그램은 사용자에게 정해진 횟수만큼 입력을 받는 것이 아닌, ESC 문자가 입력될 때까지 입출력을 처리한다. 따라서, `_start` 내부에 키 입력을 처리하고 출력하는 반복문을 작성하고, ESC 문자가 입력될 경우 파일을 저장하거나 프로그램을 종료하도록 코드를 작성해야 한다. 

키 입력을 처리하기 전, 파일을 열고 `buffer`에 파일의 내용을 읽는 코드를 먼저 작성한다. 

```nasm
_start:
	mov     rax, [rsp] ; 스택의 최상단에 argc가 저장
	cmp     rax, 2
	jne     no_argument ; 인자의 개수가 부족할 경우
	
	mov     rax, SYS_OPEN
	mov     rdi, [rsp + 16]
	mov     rsi, O_RDWR | O_CREAT ; open(filename, 파일 열기 또는 생성, 0o644)
	mov     rdx, 0o644
	syscall
	cmp     rax, 0 ; rax에는 파일 디스크립터가 저장됨
	jl      open_fail ; 파일이 열리지 않을 경우
	mov     r12, rax
	
	mov     rax, SYS_READ
	mov     rdi, r12
	mov     rsi, buffer ; read(fd, buffer, 8192)
	mov     rdx, 8192
	syscall
	cmp     rax, 0 ; rax에는 읽은 만큼의 크기가 저장됨
	jl      open_fail ; 파일이 읽히지 않을 경우
	
	mov     [buf_len], rax
```

성공적으로 파일 `open`과 `read`가 끝나면 이전에 정의했던 함수들을 호출해 터미널 설정을 변경하며, 이어서 `write` 시스템 콜을 사용해 이전에 작성되어 있던 파일의 내용을 화면에 출력한다. 아래 코드는 해당 동작을 어셈블리 코드로 작성한 것이며, 위의 코드에 이어서 작성한다. 

```nasm
_start:
	; 터미널 설정 변경
	call    saved_termios
	call    set_non_canonical_mode
	call    store_terminal
	call    clear_screen
	
	; write(1, buffer, buf_len)
	; 이전에 있던 파일의 내용을 출력
	mov     rax, SYS_WRITE
	mov     rdi, STDOUT
	mov     rsi, buffer
	mov     rdx, [buf_len]
	syscall
```


### 키 입력 처리
---
`_start` 에 이어서 키 입력 처리 코드를 작성해 보자. 

아래는 사용자의 입력을 문자 단위로 처리하는 코드이다. `key_buf`에 사용자에게 입력받은 문자 1개를 저장하고, ESC 문자가 입력된 경우 `.handle_esc` 레이블로 분기한다. 

일반적인 경우에는 `.normal_key` 레이블 이하의 코드를 실행하며, `buffer`에 입력된 문자를 추가하고 `buf_len`을 1 증가시킨 뒤 해당 문자를 화면에 출력한다. 

```nasm
.edit_loop:
    ; read(0, key_buf, 1)
    mov     rax, SYS_READ
    mov     rdi, STDIN
    mov     rsi, key_buf
    mov     rdx, 1
    syscall
    cmp     rax, 1
    jne     fatal_exit

    mov     al, [key_buf]
    cmp     al, 27 ; 사용자가 27번, 즉 ESC를 눌렀을 경우
    je      .handle_esc

.normal_key:
    mov     rbx, [buf_len]
    mov     [buffer + rbx], al
    inc     rbx
    mov     [buf_len], rbx

    ; write(1, key_buf, 1)
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    mov     rsi, key_buf
    mov     rdx, 1
    syscall
    jmp     .edit_loop
```

ESC가 입력되면, 파일 저장 및 프로그램 종료를 위해 `.handle_esc` 레이블로 분기한다. 이어서 'w'가 입력된 경우 `.save_and_continue` 레이블로 분기해 <u>파일을 저장하고 입력을 계속 진행</u>하며, 'q'가 입력된 경우 `.save_and_exit` 레이블로 분기해 <u>파일 저장과 함께 프로그램을 종료</u>한다. 만약 다른 문자가 입력된 경우, `.normal_key` 레이블로 분기해 일반 문자와 동일하게 처리한다. 

```nasm
.handle_esc:
    ; read(0, key_buf, 1)
    mov     rax, SYS_READ
    mov     di, STDIN
    mov     rsi, key_buf
    mov     rdx, 1
    syscall
    cmp     rax, 1
    jne     exit_program
    mov     al, [key_buf]

    cmp     al, 'w'
    je      .save_and_continue
    cmp     al, 'q'
    je      .save_and_exit

    jmp     .normal_key

.save_and_continue:
    call    save_file
    jmp     .edit_loop

.save_and_exit:
    call    save_file
    jmp     exit_program
```


### 전체 코드
---
앞서 작성한 코드를 기반으로 프로그램을 구현한 전체 코드는 다음과 같다. 

```nasm
section .data
    usage_msg:    db "Usage: editor <filename>", 10
    usage_msg_len equ $ - usage_msg

    open_err_msg: db "Error: cannot open file", 10
    open_err_len  equ $ - open_err_msg

    SYS_READ   equ 0
    SYS_WRITE  equ 1
    SYS_OPEN   equ 2
    SYS_CLOSE  equ 3
    SYS_LSEEK  equ 8 
    SYS_IOCTL  equ 16
    SYS_EXIT   equ 60

    STDIN      equ 0
    STDOUT     equ 1
    O_RDWR     equ 2
    O_CREAT    equ 64
    TCGETS     equ 0x5401
    TCSETS     equ 0x5402
    ICANON_FLAG equ 2
    ECHO_FLAG   equ 8

    ENTER_ALT_SCREEN: db 0x1b, "[?1049h", 0
    ENTER_ALT_SCREEN_LEN: equ $ - ENTER_ALT_SCREEN
    EXIT_ALT_SCREEN: db 0x1b, "[?1049l", 0
    EXIT_ALT_SCREEN_LEN: EQU $ - EXIT_ALT_SCREEN
    CLEAR_SCREEN: db 0x1b, "[2J", 0x1b, "[H", 0
    CLEAR_SCREEN_LEN: equ $ - CLEAR_SCREEN

section .bss
    buffer:      resb 8192
    buf_len:     resq 1
    termios_old: resb 66
    termios_new: resb 66
    key_buf:     resb 1

section .text
    global _start

_start:
    ; [1] 매개변수 개수 확인
    mov     rax, [rsp]
    cmp     rax, 2
    jne     no_argument

    ; [2] 파일 열기
    mov     rax, SYS_OPEN
    mov     rdi, [rsp+16]
    mov     rsi, O_RDWR | O_CREAT
    mov     rdx, 0o644
    syscall
    cmp     rax, 0
    jl      open_fail
    mov     r12, rax

    ; [3] 파일 읽기
    mov     rax, SYS_READ
    mov     rdi, r12
    mov     rsi, buffer
    mov     rdx, 8192
    syscall
    cmp     rax, 0
    jl      open_fail

    mov     [buf_len], rax

    ; [4] 터미널 설정 변경
    call    saved_termios
    call    set_non_canonical_mode
    call    store_terminal
    call    clear_screen

    ; [5] 파일 내용 출력
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    mov     rsi, buffer
    mov     rdx, [buf_len]
    syscall

    ; [6] 파일 편집
.edit_loop:
    mov     rax, SYS_READ
    mov     rdi, STDIN
    mov     rsi, key_buf
    mov     rdx, 1
    syscall
    cmp     rax, 1
    jne     fatal_exit

    mov     al, [key_buf]
    cmp     al, 27
    je      .handle_esc

.normal_key:
    mov     rbx, [buf_len]
    mov     [buffer + rbx], al
    inc     rbx
    mov     [buf_len], rbx

    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    mov     rsi, key_buf
    mov     rdx, 1
    syscall
    jmp     .edit_loop

.handle_esc:
    mov     rax, SYS_READ
    mov     di, STDIN
    mov     rsi, key_buf
    mov     rdx, 1
    syscall
    cmp     rax, 1
    jne     exit_program
    mov     al, [key_buf]

    cmp     al, 'w'
    je      .save_and_continue
    cmp     al, 'q'
    je      .save_and_exit

    jmp     .normal_key

    ; [7] 파일 저장
.save_and_continue:
    call    save_file
    jmp     .edit_loop

    ; [8] 파일 종료
.save_and_exit:
    call    save_file
    jmp     exit_program


saved_termios:
    mov     rax, SYS_IOCTL
    mov     rdi, STDIN
    mov     rsi, TCGETS
    lea     rdx, [termios_old]
    syscall
    test    rax, rax
    js      fatal_exit
    ret


set_non_canonical_mode:
    mov     rax, SYS_IOCTL
    mov     rdi, STDIN
    mov     rsi, TCGETS
    lea     rdx, [termios_new]
    syscall

    and     dword [termios_new + 12], ~(ICANON_FLAG | ECHO_FLAG)
    mov     byte [termios_new + 22], 0 ; VTIME
    mov     byte [termios_new + 23], 1 ; VMIN

    mov     rax, SYS_IOCTL
    mov     rdi, STDIN
    mov     rsi, TCSETS
    lea     rdx, [termios_new]
    syscall
    ret


store_terminal:
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [ENTER_ALT_SCREEN]
    mov     rdx, ENTER_ALT_SCREEN_LEN
    syscall
    ret


clear_screen:
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [CLEAR_SCREEN]
    mov     rdx, CLEAR_SCREEN_LEN
    syscall
    ret


restore_terminal:
    mov     rax, SYS_IOCTL
    mov     rdi, STDIN
    mov     rsi, TCSETS
    lea     rdx, [termios_old]
    syscall
    test    rax, rax
    js      fatal_exit

    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    lea     rsi, [EXIT_ALT_SCREEN]
    mov     rdx, EXIT_ALT_SCREEN_LEN
    syscall
    ret


save_file:
    mov     rax, SYS_LSEEK
    mov     rdi, r12
    xor     rsi, rsi
    xor     rdx, rdx
    syscall

    mov     rax, SYS_WRITE
    mov     rdi, r12
    mov     rsi, buffer
    mov     rdx, [buf_len]
    syscall
    ret


open_fail:
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    mov     rsi, open_err_msg
    mov     rdx, open_err_len
    syscall
    jmp     fatal_exit


no_argument:
    mov     rax, SYS_WRITE
    mov     rdi, STDOUT
    mov     rsi, usage_msg
    mov     rdx, usage_msg_len
    syscall
    jmp     fatal_exit

fatal_exit:
    mov     rax, SYS_EXIT
    mov     rdi, 1
    syscall
    

exit_program:
    call    restore_terminal
    mov     rax, SYS_CLOSE
    mov     rdi, r12
    syscall
    mov     rax, SYS_EXIT
    xor     rdi, rdi
    syscall
```


### 컴파일 및 실행
---
작성한 코드를 `editor.asm`으로 저장 후, **아래 명령어**를 *build.sh* 파일로 만들어 실행하면 실행 파일을 생성할 수 있다. 

```shell
#!/bin/bash
nasm -f elf64 editor.asm -o editor.o
ld editor.o -o editor
```

올바르게 코드를 작성했다면, 생성된 실행 파일은 아래 사진과 같이 실행되어야 한다. 편집하고자 하는 파일 이름을 명령줄 인수로 함께 전달하면 화면이 초기화되고 파일의 내용이 화면에 출력되며 입력이 시작된다. 

편집이 끝나고 ESC와 Q를 차례대로 입력하면 파일을 저장하고 프로그램을 종료한다. 만약 매개변수를 전달하지 않은 경우, 사용 방법을 출력하고 프로그램을 종료한다.

![[Pasted image 20251225064239.png]]
