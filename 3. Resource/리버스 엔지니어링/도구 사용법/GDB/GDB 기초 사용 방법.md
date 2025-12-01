### 파일 불러오기
GDB가 성공적으로 설치 되었다면, 터미널에서 `gdb`명령어를 입력해 GDB를 실행할 수 있다. 디버깅을 수행하기 위해 `gdb`명령어로 디버깅 터미널을 생성한 뒤 가장 먼저 해야 할 일은 디버깅 할 바이너리를 GDB에 불러 오는 것이다. 
`file`명령어는 바이너리를 GDB에 불러 와 실행하고 디버깅 할 수 있게 한다. `file` 명령어 뒤에 불러 올 바이너리 경로를 넣어 사용할 수 있고, 처음 터미널에서 `gdb`를 실행할 때 `gdb ./debugee`와 같이 인자에 디버깅 할 바이너리의 경로를 넣어도 동일한 기능을 수행한다. 
성공적으로 수행되었다면 다음과 같은 문구가 표시된다. 
```c
pwndbg> file ./debugee
Reading symbols from ./debugee...
(No debugging symbols found in ./debugee)
```
만약 되지 않는다면 해당 파일을 다운로드 받은 디렉터리에 들어가서 해당 파일을 메인 디렉터리로 옮기고 터미널을 실행하면 된다. (그래도 안 되면 권한 문제이니, 해당 파일이 있는 디렉터리에 들어가서 `chmod +x (프로그램명)`을 입력하면 파일 실행 권한이 생겨 실행 할 수 있을 것이다)
또한, `(No debugging symbols found in ./debugee)`라고 표시되는 이유는 debugee 바이너리 안에 디버깅 정보가 포함되어 있지 않기 때문이다. 디버깅 정보를 포함하기 위해서는 컴파일 할 때 `-g`옵션을 주고 컴파일해야 하지만, 해커들이 마주치는 대부분의 바이너리는 `-g`옵션 없이 컴파일한 바이너리다. 

이제 앞으로 설명할 명령어 및 기능들을 활용해 디버깅을 수행할 수 있는 상태가 되었다. 이후 내용들은 모두 `file`명령어가 성공적으로 수행됨을 가정하고 설명한다. 
### 실행 흐름 제어하기
#### run
디버거는 기본적으로 프로그램을 실행하면서 분석하는 도구로, 실행 중 특정 지점에서의 프로세스 상태를 진단하고 버그를 찾게 도와준다. 디버깅을 학습하기 이전에 GDB에서 프로그램을 어떻게 실행하는지를 먼저 배울 것이다. 
`run`명령어는 GDB에서 프로그램을 실행하는 명령어이다. `r`명령어만 입력해도 `run`과 동일하게 실행될 수 있도록 하는 단축 명령어도 존재한다. 
####  break, continue
`break`는 특정 주소에 중단점을 설정하는 기능이고, `continue`는 중단된 프로그램을 계속 실행하는 기능이다. `break`로 원하는 함수에 중단점을 설정하고, 프로그램을 계속 실행하면 해당 지점까지 멈추지 않고 실행한 다음 중단되며, 중단점은 함수명과 주소를 이용해 걸 수 있다. 정식 명령어는 `break`이지만 `b`명령어로도 동일한 작업을 수행할 수 있다. 
```c
pwndbg> b main
Breakpoint 1 at 0x401156

pwndbg> b *0x401156
Breakpoint 2 at 0x401156
```
첫 번째는 함수 이름에 직접 중단점을 거는 것이고, 두 번째는 함수 자체의 주소에 중단점을 거는 것으로 둘의 결과는 동일한 것을 확인할 수 있다. 
단, 함수 자체의 주소에 중단점을 걸려면 주소 앞에 `*`을 붙여야 한다. 
함수명을 이용해 중단점을 건 뒤 `run`명령어를 입력하면 프로그램이 실행 되다가 중단점이 걸린 `main`에서 실행이 중단되면서 다음과 같은 화면이 출력된다. 
![[Pasted image 20250725021735.png]]
![[Pasted image 20250725021808.png]]
이렇게 많은 정보가 출력되며, 이는 강의 후반부의 `context`명령어를 배울 때 설명할 것이다. 
간단하게 설명하면, 위의 사진의 경우, 프로그램에 대한 상세한 정보들을 출력해 주는 것으로 현재 실행되고 있는 [[Assembly|어셈블리 명령어]]와 그 주변의 명령어들을 보여준다. 
중단점으로 인해 실행이 중단된 상태에서 필요한 작업을 모두 했다면, `continue`명령어를 이용해 다시 실행을 이어나갈 수 있다. `c`명령어로도 동일하게 작업을 수행할 수 있다. 
![[Pasted image 20250725022238.png]]
>[!example]- Tips
>중단점을 여러 번 건너뛸 상황이 생겨 `continue`를 여러 번 수행하고 싶다면 `continue (숫자)` 또는 `c (숫자)`를 이용할 수 있다. 
>이는 단순히 `continue`명령어를 여러 번 실행하는 것은 아니며, 현재 프로그램이 중단되어 있는 위치의 중단점을 여러 번 건너 뛸 수 있게 해주는 명령어다. 
>주로 반복문 안에서 중단점을 걸었을 때 유용하게 써먹을 수 있다.

#### entry, start
Windows의 PE처럼, 리눅스는 실행파일의 형식으로 ELF(Executable and Linked Format)을 사용한다. ELF는 크게 헤더와 여러 섹션으로 구성되어 있으며 헤더에는 실행에 필요한 여러 정보가, 섹션들에는 컴파일된 기계어 코드, 프로그램 문자열을 비롯한 여러 데이터가 포함되어 있다. 
Linux는 헤더의 진입점(Entry Point, EP)이라는 필드의 명령어부터 프로그램을 실행한다. 진입점을 확인할 수 있는 명령어는 `readelf`이며, 해당 명령어로 확인해 본 결과 debugee의 진입점은 0x401050이다. 
```c
$ readelf -h debugee
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x401050
  Start of program headers:          64 (bytes into file)
  Start of section headers:          13912 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         13
  Size of section headers:           64 (bytes)
  Number of section headers:         31
  Section header string table index: 30
```
GDB의 `entry`명령어는 진입점부터 프로그램을 분석할 수 있도록 해 주는 명령어이다. 출력된 결과에서 `DISASM`의 화살표가 가리키는 주소는 `RIP`레지스터의 값으로 ==현재 실행 중인 명령어의 주소==인데, `entry`명령어를 실행하고 보면 `0x401050`주소를 가리킨다. 
![[Pasted image 20250725023457.png]]
`entry`명령어와 유사하지만 프로그램의 진입점이 아닌 `main`함수부터 분석할 수 있는 `start`명령어가 있다. 이 명령어를 수행하면 GDB에서 프로그램을 실행하고 `main`함수의 위치를 찾아 멈춰준다. 
 GDB가 메인 함수를 찾을 수 없는 경우, `entry`명령어와 동일하게 동작한다. 
 ![[Pasted image 20250725023739.png]]
 ![[Pasted image 20250725023805.png]]

### 실행 흐름 추적하기
관찰하고자 하는 함수의 중단점에 도달했다면, 그 지점부터는 명령어를 한 줄씩 실행하며 분석해야 한다. 이때 사용하는 명령어로 `ni`, `si`가 있으며, `ni`와 `si`는 모두 어셈블리 명령어를 한 줄씩 실행한다는 공통점이 있다. 
그러나 [[x64dbg 사용 방법]]처럼 `call`명령어 뒤에 함수를 호출하는 경우, `ni`는 x64dbg의 F8처럼 함수의 내부로 들어가지 않지만, `si`는 x64dbg의 F7처럼 함수의 내부로 들어간다는 차이점이 있다. 
이를 확인하기 위해 main 함수에서 `printf()`를 호출하는 지점인 `0x401190`에 중단점을 걸고 실행하자.
```c
pwndbg> b *0x401190
Breakpoint 1 at 0x401190
pwndbg> r

Breakpoint 1, 0x0000000000401190 in main ()
LEGEND: STACK | HEAP | CODE | DATA | WX | RODATA
[중략]
──────────────────────────[ DISASM / x86-64 / set emulate on ]───────────────────────────
 ► 0x401190 <main+66>     call   printf@plt                  <printf@plt>
        format: 0x402004 ◂— '%d + %d = %d\n'
        vararg: 2

   0x401195 <main+71>     mov    eax, 0                 EAX => 0
   0x40119a <main+76>     leave
   0x40119b <main+77>     ret

   0x40119c <_fini>       endbr64
   0x4011a0 <_fini+4>     sub    rsp, 8
   0x4011a4 <_fini+8>     add    rsp, 8
   0x4011a8 <_fini+12>    ret

   0x4011a9               add    byte ptr [rax], al
   0x4011ab               add    byte ptr [rax], al
   0x4011ad               add    byte ptr [rax], al
[중략]
pwndbg>
```
#### ni (next instruction)
`ni`를 입력하면 아래 사진과 같이 `printf()`바로 다음인 `0x401195`로 `RIP`레지스터가 이동한 것을 확인할 수 있다.
![[Pasted image 20250725024929.png]]
#### si (step into)
이번에는 `ni`를 실행하지 않고, `printf()`를 호출하는 지점까지 다시 프로그램을 실행시킨 뒤, `ni`와 `si`의 차이를 알아보기 위해 `si`명령어를 입력해 보자. ![[Pasted image 20250725025409.png]]
그러면 이번에는 아까와 다르게 `printf()` 내부로 `RIP`가 이동한 것을 확인할 수 있다. 
따라서, 프로그램을 분석하다가 어떤 함수의 내부가 궁금할 때는 `si`를, 그렇지 않을 때는 `ni`를 사용한다.
#### finish
`si`로 함수 내부에 들어가서 분석할 만한 부분을 모두 분석했는데 함수의 규모가 커서 `ni`로는 원래 실행 흐름으로 돌아가기 어려울 수 있다. 이럴 때는 `finish`명령어를 사용해 함수의 끝까지 한 번에 실행할 수 있다. 
아래는 위에서 `si`로 `printf()`내부로 진입한 상태에서 `finish`명령어를 사용하는 예시이다. ![[Pasted image 20250725030015.png]]
이러면,`RIP`가 `0x401195`에 위치하게 되어 `printf()`를 빠져나왔다는 것을 알 수 있다. 

### 디버깅을 위해 자주 사용하는 명령어들
#### info
말 그대로 정보를 보여주는 명령어로, 대표적이고 자주 쓰이는 기능으로는 `info register (i r)`과 `info breakpoint (i b)`가 존재한다. 
`info register`명령어는 프로그램이 실행 중일 때 사용하는 명령어로 이런 상황 외에 레지스터에 들어 있는 값을 바로 사용하고 싶은 경우, `$(레지스터 이름)`의 형식으로 바로 사용할 수 있다. 
가령, `RDI`레지스터가 가지고 있는 값이 특정 코드의 주소일 경우, 해당 주소에 중단점을 걸고 싶은 경우는 `pwndbg> b $rdi`명령어를 사용한다. 
`info breakpoint`명령어는 프로그램의 실행 여부와 상관 없이 사용할 수 있는 명령어이며, `break`명령어로 설정한 중단점들을 모두 확인할 수 있다. 
```c
pwndbg> b main
Breakpoint 1 at 0x401156
pwndbg> b printf
Breakpoint 2 at 0x401040
pwndbg> i b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000401156 <main+8>
2       breakpoint     keep y   0x0000000000401040 <printf@plt>
```
각 중단점마다 번호가 부여된 것을 알 수 있고, 이 번호를 이용해 특정 중단점을 비활성화/활성화하거나 삭제할 수 있다. 
* 중단점 비활성화: `disable (중단점 번호)`명령어로 사용, 이후 `enable`로 활성화 할 때까지 해당 중단점에서 프로그램이 멈추지 않음
* 중단점 활성화: `enable (중단점 번호)`명령어로 사용
* 중단점 삭제: `delete (중단점 번호)`명령어로 사용, `d`라는 단축 명령어로도 사용할 수 있음
`disable, enable, delete`모두 중단점 번호 없이 단일로 사용할 경우 모든 중단점에 대해 수행한다. 

#### disassemble
프로그램의 코드는 기계어로 이루어져 있기 때문에, GDB를 비롯한 모든 디버거들은 기계어를 디스어셈블(Disassemble)하는 기능을 기본적으로 탑재하고 있다. 추가로, pwndbg에는 디스어셈블된 결과를 가독성 좋게 출력해 주는 기능을 지원한다. 
`disassemble`은 GDB가 기본적으로 지원하는 디스어셈블 명령어로, 아래 사진과 같이 함수 이름을 인자로 전달하면 해당 함수가 반환될 때까지 전부 디스어셈블하여 보여준다. `disass`로도 가능하다. ![[Pasted image 20250725035004.png]]
`u, nearpc, pdisass`는 pwndbg에서 제공하는 디스어셈블 명령어로, 디스어셈블된 코드를 가독성 좋게 출력해 준다. 
#### x (examine)
프로그램을 분석하다 보면 가상 메모리에 존재하는 특정한 값을 관찰해야 할 때가 있다. 이를 위해 GDB에서는 기본적으로 `x`라는 명령어를 제공하며, 이를 이용하면 특정 주소에서 원하는 길이만큼의 데이터를 특정 포맷으로 인코딩해서 볼 수 있다. 

| 포맷              | 크기                  |
| --------------- | ------------------- |
| `x`: 16진수       | `b`: byte, 1바이트     |
| `o`: 8진수        | `h`: halfword, 2바이트 |
| `d`: 10진수       | `w`: word, 4바이트     |
| `u`: 부호 없는 10진수 | `g`: giant, 8바이트    |
| `t`: 2진수        |                     |
| `f`: float형     |                     |
| `a`: 주소         |                     |
| `i`: 명령어        |                     |
| `c`: 문자         |                     |
| `s`: 문자열        |                     |
이 표의 포맷과 크기 중 원하는 대로 조합해서 `x/(포맷과 크기)` 형태로 명령어를 사용할 수 있다.
값 여러 개를 한 번에 보기를 원한다면 `x/(개수)(포맷과 크기)`형태로 명령어를 사용할 수 있다. 
이렇게 만들어진 명령어 뒤에 출력을 원하는 주소를 넣어 사용한다. 
1. `RSP`부터 80바이트를 8바이트씩 16진수 형식으로 출력
	![[Pasted image 20250725040902.png]]
2. `RIP`부터 5줄의 어셈블리 명령어 출력
	![[Pasted image 20250725040847.png]]
3. `0x400000`의 문자열 출력
```
pwndbg> x/s 0x400000
0x400000:       "\177ELF\002\001\001"
```
단, 가상 메모리는 프로그램이 실행되기 전에는 할당되지 않기 때문에 `x`명령어는 프로그램 실행 도중에만 사용할 수 있다. 
#### telescope
`telescope`는 pwndbg가 제공하는 강력한 메모리 덤프 기능으로, 특정 주소의 메모리 값을 보여주는 것에서 그치지 않고 메모리가 참조하고 있는 주소를 탐색해 값을 보여준다. 
```c
pwndbg> tele $rsp
00:0000│ rsp  0x7fffffffc228 —▸ 0x7ffff7a05b97 (__libc_start_main+231) ◂— mov    edi, eax
01:0008│      0x7fffffffc230 ◂— 0x1
02:0010│      0x7fffffffc238 —▸ 0x7fffffffc308 —▸ 0x7fffffffc557 ◂— '/home/dreamhack/debugee'
03:0018│      0x7fffffffc240 ◂— 0x100008000
04:0020│      0x7fffffffc248 —▸ 0x4004e7 (main) ◂— push   rbp
05:0028│      0x7fffffffc250 ◂— 0x0
06:0030│      0x7fffffffc258 ◂— 0x71eb993d1f26e436
07:0038│      0x7fffffffc260 —▸ 0x400400 (_start) ◂— xor    ebp, ebp
```

#### vmmap
`vmmap`은 가상 메모리의 레이아웃을 보여주는 명령어로, 어떤 파일이 매핑, 즉 메모리에 적재된 영역인 경우 해당 파일의 경로까지 보여준다. 이 명령어는 프로그램이 실행된 상태에서 이용할 수 있다. 
![[Pasted image 20250725041645.png]]
>[!example]- 파일 매핑이란
>어떤 파일을 메모리에 적재하는 것을 파일 매핑이라고 한다.
>리눅스에서는 ELF를 실행할 때, 먼저 ELF의 코드와 여러 데이터를 가상 메모리에 매핑하고, 해당 ELF에 링크된 공유 오브젝트(Shared Object, so)를 추가로 메모리에 매핑한다. 
>공유 오브젝트는 Windows의 DLL과 비슷한 개념으로, 자주 사용되는 함수들을 미리 컴파일해둔 것이며 C언어의 `printf()`, `scanf()`와 같은 함수들이 리눅스에서는 Libc(Library C)에 구현되어 있다. 
>공유 오브젝트에 이미 구현된 함수를 호출할 때는 매핑된 메모리에 존재하는 함수를 대신 호출한다. 

#### backtrace 
해당 명령어는 콜 스택을 확인할 수 있는 명령어로, 만약 어떤 함수에 전달된 인자에 문제가 발생해 버그가 났다면 이 인자가 어떤 함수로부터 왔는지를 거슬러 올라가면서 찾아야 하는데, 그 때 유용하다. 
**콜 스택 (Call Stack)** 이란 프로그램이 실행되는 동안 함수가 호출되는 순서를 저장하는 구조이다. 프로그램이 실행되면서 하나의 함수가 다른 함수를 호출할 때마다 새로운 함수 호출 정보가 추가되며, 함수 실행이 끝나면 가장 최근에 호출된 함수부터 순차적으로 제거된다.
아래 사진은 `add()`를 호출한 뒤 콜 스택을 `backtrace`, 또는 `bt`명령어를 통해 확인하는 예시이다. 
![[Pasted image 20250725043159.png]]

#### dump memory
현재 프로세스의 메모리 상태를 파일로 저장하고 싶을 때 사용하는 명령어로, 
`dump memory (저장할 파일명) (시작 주소) (끝 주소)`로 사용할 수 있다. 
예를 들어, 프로그램의 코드 영역을 덤프하고 싶다면 다음과 같이 `dump memory`를 사용할 수 있다. 
```c
pwndbg> entry
[중략]
pwndbg> vmmap
LEGEND: STACK | HEAP | CODE | DATA | WX | RODATA
             Start                End Perm     Size Offset File (set vmmap_prefer_relpaths on)
          0x400000           0x401000 r--p     1000      0 debugee
          0x401000           0x402000 r-xp     1000   1000 debugee
		  // 코드 영역은 기본적으로 읽기 권한과 실행 권한이 부여되어 있기 때문에, debugee 파일이 매핑된 영역 중 읽기 권한(r)과 실행 권한(x)이 부여된 영역이 debugee 바이너리의 코드 영역이 된다.

          0x402000           0x403000 r--p     1000   2000 debugee
          0x403000           0x404000 r--p     1000   2000 debugee
          0x404000           0x405000 rw-p     1000   3000 debugee
    0x7ffff7da1000     0x7ffff7da4000 rw-p     3000      0 [anon_7ffff7da1]
    0x7ffff7da4000     0x7ffff7dcc000 r--p    28000      0 /usr/lib/x86_64-linux-gnu/libc.so.6
    0x7ffff7dcc000     0x7ffff7f54000 r-xp   188000  28000 /usr/lib/x86_64-linux-gnu/libc.so.6
    0x7ffff7f54000     0x7ffff7fa3000 r--p    4f000 1b0000 /usr/lib/x86_64-linux-gnu/libc.so.6
    0x7ffff7fa3000     0x7ffff7fa7000 r--p     4000 1fe000 /usr/lib/x86_64-linux-gnu/libc.so.6
    0x7ffff7fa7000     0x7ffff7fa9000 rw-p     2000 202000 /usr/lib/x86_64-linux-gnu/libc.so.6
    0x7ffff7fa9000     0x7ffff7fb6000 rw-p     d000      0 [anon_7ffff7fa9]
    0x7ffff7fbd000     0x7ffff7fbf000 rw-p     2000      0 [anon_7ffff7fbd]
    0x7ffff7fbf000     0x7ffff7fc3000 r--p     4000      0 [vvar]
    0x7ffff7fc3000     0x7ffff7fc5000 r-xp     2000      0 [vdso]
    0x7ffff7fc5000     0x7ffff7fc6000 r--p     1000      0 /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
    0x7ffff7fc6000     0x7ffff7ff1000 r-xp    2b000   1000 /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
    0x7ffff7ff1000     0x7ffff7ffb000 r--p     a000  2c000 /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
    0x7ffff7ffb000     0x7ffff7ffd000 r--p     2000  36000 /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
    0x7ffff7ffd000     0x7ffff7fff000 rw-p     2000  38000 /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
    0x7ffffffde000     0x7ffffffff000 rw-p    21000      0 [stack]
pwndbg> dump memory code_section 0x401000 0x402000
pwndbg>
```
`entry`명령어로 프로그램을 실행한 뒤, `vmmap`명령어로 프로그램의 코드 영역의 시작 주소와 끝 주소를 얻는다. 이후 `dump memory`명령으로 덤프를 수행한다. 
실행하면 code_section이라는 파일명으로 파일이 생성된 것을 확인할 수 있다. 

#### context
프로그램은 실행되면서 레지스터를 비롯한 여러 메모리에 접근한다. 따라서 디버거를 이용해 프로그램의 실행 과정을 자세히 관찰하려면 컴퓨터의 각종 메모리를 한눈에 파악할 수 있는 것이 좋다. 
이를 pwndbg에서는 `context`, 또는 `ctx`라는 명령어를 이용해 가독성 있게 표현할 수 있다. 
`context`는 크게 4개의 영역으로 구분된다. 
1. `REGISTERS`: 레지스터의 상태
2. `DISASM`: `RIP`부터 여러 줄에 걸쳐 디스어셈블된 결과
3. `STACK`: `RSP`부터 여러 줄에 걸친 스택의 값
4. `BACKTRACE`: 현재 `RIP`에 도달할 때까지 어떤 함수들이 호출되었는지
![[Pasted image 20250725045152.png]]
![[Pasted image 20250725045229.png]]
![[Pasted image 20250725045258.png]]
이들은 어셈블리를 실행할 때마다 갱신된다. 

#### set
프로세스의 메모리 상태를 변경할 수 있는 명령어로, 주로 레지스터 값을 변경하거나 특정 주소의 메모리의 값을 변경하기 위해 사용한다. 이 경우, 프로그램이 실행 중인 상태에서만 동작한다. 
`set (주소 또는 레지스터) = (변경할 값)`의 형식으로 사용한다. 
1. 레지스터의 값을 변경하는 경우
```c
pwndbg> set $rax = 0
pwndbg> set $rsp = $rbp
```
이 경우, `info register`, 또는 `i r` 명령어를 사용해 바뀐 레지스터 값을 확인할 수 있다. 
2. 특정 주소의 값을 변경하는 경우
```c
pwndbg> set *(unsigned int*)0x400000 = 10
pwndbg> set *(float*)0x400010 = 3.14
```
첫 번째는 메모리 주소 0x400000을 `unsigned int *` 형으로 역참조한 후 여기에 정수 10을 저장한다. 따라서 0x400000 주소부터 시작하여 4바이트의 메모리 공간에 10을 기록하게 된다. 
두 번째는 메모리 주소 0x400010을 `float *`형으로 역참조한 후 여기에 부동 소수점 값 3.14를 저장한다. 따라서 0x400010 주소부터 시작해 4바이트의 메모리 공간에 3.14을 기록하게 된다. 

이후 `examine`기능으로 바뀐 메모리 값을 확인할 수 있다. 
```c
pwndbg> x/wu 0x400000
0x400000:       10
pwndbg> x/wf 0x400010
0x400010:       3.1400001
```