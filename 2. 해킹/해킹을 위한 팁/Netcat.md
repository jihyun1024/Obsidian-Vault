---
tags:
  - Dreamhack
  - Beginner
---
## Netcat (nc)
---
현대 거의 모든 네트워크 통신은 네트워크 소켓을 통해 이루어진다. [[보안네트워크 프로그래밍]] 수업에서도 배웠듯, 소켓은 통신을 위한 가장 작은 단위의 프로토콜이며, 인터넷 브라우저를 사용할 때도 낮은 레벨에서는 80번, 443번 포트로 TCP 통신을 한다는 말을 들을 수 있다. 

보안을 공부하는 많은 실습의 경우, 서버에서 특정 포트를 통해 서비스를 동작시키는 환경을 구성한다. 이런 경우, 클라이언트가 그 프로그램과 통신하기 위해 Netcat (nc)이라는 도구를 사용한다.

![[Pasted image 20260123005155.png]]


### Netcat 설치
---
Ubuntu에서는 아래의 명령어로 설치한다. 

```shell
sudo apt update && sudo apt install netcat
```


### Netcat 사용법
---
```shell
$ nc
usage: nc [-46CDdFhklNnrStUuvZz] [-I length] [-i interval] [-M ttl]
      [-m minttl] [-O length] [-P proxy_username] [-p source_port]
      [-q seconds] [-s sourceaddr] [-T keyword] [-V rtable] [-W recvlimit]
      [-w timeout] [-X proxy_protocol] [-x proxy_address[:port]]
      [destination] [port]
$
```

가장 간단한 사용 방식은 `nc hostname(ip) port`이다. 

아래는 `google.com`에 80번 포트로 연결을 한 것이며 80번 포트는 HTTP 통신에 사용되는 포트이다. 해당 포트로 연결을 시도한 후 `GET /http/1.1`을 입력한 후 내 입력이 끝났다는 의미로 `Enter` 키를 입력하면, 서버에서 이 request에 해당하는 response를 전송함을 확인할 수 있다. 

```shell
$ nc google.com 80
GET / HTTP/1.1  
​
HTTP/1.1 200 OK
Date: Thu, 01 Dec 2022 02:30:32 GMT
Expires: -1
Cache-Control: private, max-age=0
Content-Type: text/html; charset=ISO-8859-1
Cross-Origin-Opener-Policy-Report-Only: same-origin-allow-popups; report-to="gws"
...
```

더 자세한 정보는 Google에서 직접 구글링하며 알아가 보자.


## 실습: Welcome-Beginners
---
Dreamhack에서 제공하는 많은 실습 문제와 워게임들은 `nc` 명령어로 접속해 푸는 형태가 많다. 웹 서비스에 접속해서 푸는 형태의 워게임은 브라우저를 사용해 접속할 수 있지만, <u>ELF 바이너리가 서비스로 등록되어 제공되는 워게임의 경우 Netcat으로 접속해 해결해야 한다.</u> 따라서 `nc` 명령어의 사용법은 알고 있어야 한다. 

이번에는 Netcat을 사용해 Dreamhack 워게임이 제공하는 접속 정보에 접속하는 과정을 실습해 본다. [이 링크](https://dreamhack.io/wargame/challenges/812)를 통해 문제 페이지에 접속하면 된다. 

![[Pasted image 20260123012911.png]]

### 문제 정보
---
가장 먼저, **문제 정보**로 주어진 **Description**을 읽어야 한다. 

![[Pasted image 20260123012945.png]]
보통 문제에 대한 설명이나 힌트, 참고 링크 등이 제공되지만, 아무것도 제공되지 않을 수 있다. 


### 접속 정보
---
Dreamhack 워게임의 대부분은 VM을 통해 문제 환경에 접속한다. 

![[Pasted image 20260123013101.png|350]]![[Pasted image 20260123013110.png|350]]
잘 안 보이긴 하지만 왼쪽 사진의 빨간 네모로 표시된 **서버 생성하기** 버튼을 누르면 VM이 요청되어 오른쪽 사진의 빨간 네모처럼 VM 접속 정보가 출력된다. 

접속 정보의 상단에는 아래처럼 Host, Port 정보를 출력하고, 하단에는 접속 방법을 출력한다. 

```
Host: host3.dreamhack.games
Port: 14309/tcp → 31337/tcp
```

위의 경우, 아래 명령어로 문제의 VM에 접속할 수 있다. 

```shell
$ nc host3.dreamhack.games 14309
http://host3.dreamhack.games:14309/
```


### 접속 방법
---
문제에 접속하는 방법은 크게 두 가지로 나뉜다. 웹 서비스의 경우 `http`로 시작하는 링크(위의 명령어에서 두 번째에 해당)로 접속하고, 아닌 경우 `nc`로 접속한다. 

- `http`로 접속하기: 브라우저에서 `http://Host:Port` 링크의 문제 환경에 접속하며, 대부분의 웹 해킹 문제는 이 방법을 사용한다. 
- `nc`로 접속하기: `nc Host Port` 명령어를 통해 문제 환경에 접속하며, 대부분의 Pwnable 문제는 이 방법을 사용한다. 

실습 문제는 `nc` 명령어를 사용해 접속할 수 있다. 아래 사진은 `nc` 명령어를 사용해 Linux 터미널에서 실행한 모습이며, 문제 환경에 접속해 "Dreamhack"을 입력하면 플래그가 출력된다. 

![[Pasted image 20260123013851.png]]
### 문제 파일 다운로드
---
![[Pasted image 20260123013912.png]]

다시 실습 문제 페이지로 돌아가서, **문제 파일 받기** 버튼을 클릭하면 압축 파일이 다운로드된다. 압축을 해제하면 문제를 푸는 데 필요한 소스 코드 등의 파일이 있으며, 문제를 분석하고 플래그를 얻기 위해서는 주어진 파일들을 잘 살펴보는 과정이 필요하다. 

실습 문제 파일을 다운로드하고 압축을 해제하면 `chall.c` 파일이 있으며, 이번 문제는 단순히 플래그를 출력하는 코드이니 자세히 볼 필요는 없지만, **앞으로 문제에서 소스 코드가 주어지면 분석을 통해 어떤 동작을 수행하는지 파악하는 것이 필수적이다.**

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FLAG_SIZE 0x45

void init() {
	setvbuf(stdin, 0, 2, 0);
	setvbuf(stdout, 0, 2, 0);
}

int main(void) {
    int fd;
    char *flag;

    init();

    // read flag
    flag = (char *)malloc(FLAG_SIZE);
    fd = open("./flag", O_RDONLY);
    read(fd, flag, FLAG_SIZE);

    char cmp_str[10] = "Dreamhack";
    char inp_str[10];   
    printf("Enter \"Dreamhack\" : ");
    scanf("%9s", inp_str);

    if(strcmp(cmp_str, inp_str) == 0){
        puts("Welcome Beginners!");
        // print flag
        puts(flag);
    }
    
    return 0;
}
```


### 플래그 제출
---
앞서 플래그를 획득했다면, 플래그는 **Flag 입력** 칸에 제출한다. Dreamhack 공식 워게임의 플래그는 기본적으로 `DH{해시값}` 형태로, `DH{}` 까지 포함한 모든 문자열을 복사해 **Flag 입력** 칸에 넣고 제출하기 버튼을 눌러 제출하면 된다. 

![[Pasted image 20260123014417.png]]
