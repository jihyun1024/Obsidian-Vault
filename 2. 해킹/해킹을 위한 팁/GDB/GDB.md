---
tags:
  - Dashboard
---
GDB(GNU DeBugger)는 리눅스의 대표적인 디버거로, 오픈 소스로 개발되어 무료로 설치할 수 있으며 역사가 오래된 만큼 다양한 플러그인들이 개발되어 있다. Ubuntu 환경에서는 기본적으로 설치되어 있지만, GDB가 설치되지 않았다면 `sudo apt-get install gdb`명령어로 설치할 수 있다. 
GDB는 오픈소스이고 역사가 오래되어 사용할 수 있는 플러그인들도 많다. GDB의 플러그인 중에서 바이너리 분석 용도로 많이 사용되는 플러그인들은 다음과 같다. 
* **gef**: https://github.com/hugsy/gef
* **peda**: https://github.com/longld/peda
* **pwngdb**: https://github.com/scwuaptx/Pwngdb
* **pwndbg**: https://github.com/pwndbg/pwndbg

이 페이지에서는 pwndbg를 기준으로 설명할 것이다. pwndbg 외의 플러그인을 설치하고 싶다면 위의 Github 페이지에 있는 설치 가이드를 따라서 진행하면 된다.

pwndbg는 터미널에서 다음의 명령어를 수행하여 설치할 수 있다.
```
git clone https://github.com/pwndbg/pwndbg
cd pwndbg
./setup.sh
```
git이 설치되지 않았다면, git을 먼저 설치해야 하며, `sudo apt-get install git` 으로 설치할 수 있다.
GDB 설치를 마친 후, 터미널에 `gdb`를 입력했을 때 다음과 같이 `pwndbg>`라고 나오면 설치에 성공한다.
```
$ gdb
GNU gdb (Ubuntu 12.1-0ubuntu1~22.04) 12.1
Copyright (C) 2022 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
pwndbg: loaded 139 pwndbg commands and 49 shell commands. Type pwndbg [--shell | --all] [filter] for a list.
pwndbg: created $rebase, $ida GDB functions (can be used with print/break)
------- tip of the day (disable with set show-tips off) -------
The $heap_base GDB variable can be used to refer to the starting address of the heap after running the heap command
pwndbg>
```

이제 GDB를 설치했으니, 예제 바이너리를 디버깅하며 사용법을 익혀보겠다. 내가 사용법을 테스트할 때는 Kali Linux에서 실행했다. 예제 바이너리는 [여기 (바로 다운로드가 실행됨)](https://dreamhack-lecture.s3.amazonaws.com/uploads/reversing/debugee)에서 다운로드 받을 수 있다. 
아래는 예제 바이너리의 소스 코드이다. 
```c
//gcc -o debugee debugee.c -no-pie
#include <stdio.h>
int add(int a, int b) {
	return a + b;
}

int main() {
	int a = 2;
	int b = 3;
	printf("%d + %d = %d\n", a, b, add(a, b));
	return 0;
}
```
단, 같은 코드를 컴파일해도 다른 바이너리가 나올 수 있다. 같은 gcc 컴파일러로 컴파일을 수행하더라도 gcc의 버전은 매우 다양하기에 환경에 따라 컴파일된 바이너리의 어셈블리 언어나 주소 등이 조금씩 달라질 수 있다. 따라서 원활한 실습을 위해서는 위에 제공한 링크를 통해 미리 컴파일된 바이너리를 다운로드해서 실습하는 것을 권장한다. 

### 관련 문서
[[GDB 기초 사용 방법]]
