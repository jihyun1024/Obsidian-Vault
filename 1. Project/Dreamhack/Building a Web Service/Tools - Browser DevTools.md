## 들어가며
---
### 들어가며
---
웹 개발을 할 때는 프론트엔드의 Javascript나 백엔드 API에서 버그가 발생하는 경우가 많고, 적용한 CSS의 꼬라지가 맘에 들지 않을 때도 있다. 이런 경우, 코드 에디터로 원하는 부분을 변경하고 브라우저로 결과를 확인해야 하는데, 그래도 만족스럽지 않다면 코드 에디터와 브라우저를 자주 번갈아가며 수정을 반복해야 한다. 특히, CSS는 변경 결과가 어떻게 될지 쉽게 예상이 가지 않아 수정 과정이 더욱 번거로워지기도 한다. 

브라우저 **개발자 도구(DevTools)** 는 HTML과 CSS 코드를 브라우저에서 직접 수정할 수 있으며, Javascript 코드를 대상으로 한 디버거 또한 제공해 이런 불편함을 덜어주고, 서버와 오가는 HTTP 패킷도 자세하게 보여줘 프로토콜 상에서 발생하는 문제 또한 쉽게 확인할 수 있다.

개발자 도구가 웹 서비스를 진단하는 데 도움이 되는 도구인 만큼, 웹의 취약점을 이용하려는 공격에도 유용하게 사용될 수 있다. 

이번 강의에서는 웹 해킹에 자주 사용되는 개발자 도구의 기능에 대해 배우고, 간단한 실습으로 개발자 도구의 사용법도 익혀 본다. 


## 개발자 도구
---
### 개발자 도구
---
개발자 도구를 Chrome에서 실행하려면 F12를 누른다. 그러면 다음과 같은 개발자 도구 창이 브라우저에 나타나며, 아래 스크린샷에서 개발자 도구의 간단한 레이아웃 정보를 확인할 수 있다.

![[Pasted image 20251231011623.png]]

- Elements 옆의 두 아이콘
	- 요소 검사(Inspect)
	- 디바이스 툴바(Device Toolbar)
- 기능 패널
	- **Elements**: 페이지를 구성하는 HTML 검사
	- **Console**: Javascript를 실행하고 결과를 확인
	- **Sources**: HTML, CSS, JS 등 페이지를 구성하는 리소스를 확인 및 디버깅
	- **Network**: 웹 서버와 오고 가는 데이터를 확인
	- Performance
	- Memory
	- **Application**: 쿠키를 포함해 웹 어플리케이션과 관련한 데이터 확인
	- Security
	- Lighthouse
- Lighthouse 오른쪽의 아이콘: 현재 페이지에서 발생한 에러 및 경고 메시지
- 톱니바퀴 모양 아이콘 & 점 세 개: 개발자 도구 설정


### 요소 검사
---
요소 검사를 활용하면, <u>특정 요소의 대략적인 정보를 확인하고, 이와 관련한 코드를 쉽게 찾을 수 있다.</u> 

![[Pasted image 20251231013953.png]]

요소 검사 버튼을 누르고 웹 페이지에서 원하는 요소(로고, 사진, 글자 등 아무거나)에 마우스를 올리면 대상의 정보가 출력된다. 또한 그 상태에서 그 요소를 클릭하면 요소와 관련한 HTML 코드가 하이라이팅되어 쉽게 확인할 수 있다.

![[Pasted image 20251231014126.png]]


### 디바이스 툴바
---
**디바이스 툴바(Device Toolbar)** 를 활용하면 <u>현재 브라우저의 화면 비율, User-Agent를 원하는 값으로 설정할 수 있다.</u>

현대에는 웹에 접속할 수 있는 디바이스가 컴퓨터뿐만 아니라, 노트북, 태블릿, 스마트폰 등으로 점점 다양해지고 있으며, 웹 페이지가 렌더링되어야 할 화면 비율도 그 디바이스에 맞춰서 다양해지고 있다. 개발자는 이런 다양한 사용자들에게도 공평하게 좋은 UX를 줘야 하기 때문에, 자신이 개발한 웹 서비스가 다른 장치에서도 잘 작동하는지 점검해봐야 한다.

이 때 디바이스 툴바를 이용하면 이런 점검을 하나의 디스플레이에서도 쉽게 진행할 수 있다.

![[Pasted image 20251231014542.png]]


### Elements
---
#### HTML 읽기
현재 페이지를 구성하는 HTML의 코드를 읽을 수 있다. 

![[Pasted image 20251231014837.png]]

#### HTML 수정
코드를 선택한 상태로 F2를 누르거나 더블 클릭하면 이를 수정할 수 있다. 이때 요소 검사 기능을 같이 사용하면 수정할 대상이 될 코드를 빠르게 선택할 수 있어 편리하다.

![[Pasted image 20251231014945.png]]


### Console
---
**콘솔(Console)** 은 <u>프론트엔드의 자바스크립트 코드에서 발생한 각종 메시지를 출력하고, 사용자가 입력한 자바스크립트 코드를 실행</u>하는 도구이다. 

자바스크립트로 웹 개발을 할 때, `console` 오브젝트에는 개발자 도구의 콘솔에 접근할 수 있는 함수가 미리 정의되어 있다. 코드를 작성하면서 어떤 함수의 값을 중간중간에 출력하고 싶다면 `console.log` 등을 유용하게 써먹을 수 있다. 

아래는 **NodeJS**의 `console` 오브젝트이다. 

```javascript
> console
Console {
  log: [Function: bound consoleCall],
  debug: [Function: bound consoleCall],
  info: [Function: bound consoleCall],
  dirxml: [Function: bound consoleCall],
  warn: [Function: bound consoleCall],
  error: [Function: bound consoleCall],
  ...
  context: [Function: context],
  [Symbol(counts)]: Map {},
  [Symbol(kColorMode)]: 'auto' }
```

콘솔의 단축키는 Windows나 Linux에서는 Ctrl + Shift + J이며, MacOS에서는 Option + Cmd + J로, 콘솔을 사용하면 브라우저에서 자바스크립트를 실행하고 결과를 확인할 수 있다.

단축키로 콘솔을 열고, 아래 내용을 콘솔에 입력해 보자.

```
// "hello" 문자열을 출력하는 alert 함수를 실행합니다.
alert("hello");
// prompt는 popup box로 이용자 입력을 받는 함수입니다.
// 이용자가 입력한 데이터는 return value로 설정됩니다.
var value = prompt('input')
// confirm 는 확인/취소(yes/no)를 확인하는 이용자로부터 입력 받는 함수입니다.
// 이용자의 선택에 따라 Boolean(true/false)타입의 return value를 가집니다.
var true_false = confirm('yes or no ?');
// document.body를 변경합니다.
document.body.innerHTML = '<h1>Refresh!</h1>';
// document.body에 새로운 html 코드를 추가합니다.
document.body.innerHTML += '<h1>HI!</h1>';
```

위의 내용을 입력하면, 브라우저에서 실제 메시지가 출력되어 Javascript를 실행 가능하다.


### Sources
---
Sources에서는 현재 페이지를 구성하는 웹 리소스들을 확인할 수 있다. 

![[Pasted image 20251231015912.png]]

- 빨간색: 현재 페이지의 리소스 파일 트리, 파일 시스템
- 주황색: 선택한 리소스의 상세 정보
- 연두색: 디버깅 정보
	- Watch: 원하는 자바스크립트 식을 입력하면, 코드 실행 과정에서 해당 식의 값 변화를 확인 가능
	- Call Stack: 함수들의 호출 순서를 스택의 형태로 보여 주며, `A, B, C` 순서대로 함수가 호출되어 현재 `C` 내부의 코드를 실행하고 있다면, 가장 위에는 `C`, 가장 아래에는 `A` 위치
	- Scope: 정의된 모든 변수들의 값을 확인
	- Breakpoints: 브레이크포인트(Breakpoint, 중단점)들을 확인, 각각을 활성화/비활성화

#### Sources: Debug
Source 탭에서는 원하는 자바스크립트를 디버깅할 수도 있다. 

아래의 코드는 사용자가 입력한 `name`, `num`에 따라 실행 흐름이 바뀌는 코드인데, 이를 다음과 같은 방법으로 디버깅할 수 있다. [실습 페이지](https://dreamhack-lecture.s3.ap-northeast-2.amazonaws.com/uploads/web-devtools/debug.html)에서 직접 실습해 보자.

```html
</!DOCTYPE html>
<html>
<head>
    <title>JS Debug</title>
</head>
<body>
    <input type='text' id='input-name' placeholder='name'><br/>
    <input type='text' id='input-num' placeholder='num'><br/>
    <!-- 버튼 클릭 시 button_click함수가 실행됩니다. -->
    <input type='button' onclick='button_click()' value="Click">
<script>
    /*
     name과 num에 대한 변수를 검증하는 함수입니다.
     name이 'dreamhack', num이 31337인 경우 "congratulations !" 문자열을 출력합니다.
    */
    function compare(name, num){
        if(name == 'dreamhack'){
            if(num == 31337){
                console.log("congratulations !");
                return;
            }
        }
        console.log("No !");
    }
    /*
     버튼 클릭 시 실행되는 함수입니다.
     'input-name', 'input-num'의 값을 가져와 compare함수를 실행합니다.
    */
    function button_click() {
        var name = document.getElementById('input-name').value;
        var num = parseInt(document.getElementById('input-num').value);
        compare(name, num);
    }
</script>
</body>
</html>
```

1. 원하는 코드 라인을 클릭해 해당 라인에 중단점을 설정한다.
2. 임의의 데이터를 입력하면 해당 중단점에서 실행이 멈춘다.
3. Scope에서 현재 할당된 변수들을 확인하고 값을 변경할 수 있다.


### Network
---
서버와 오가는 데이터를 확인할 수 있다. 

![[Pasted image 20251231021204.png]]

- 빨간색: 로깅 중지, 로그 전체 삭제
- 주황색: 로그 필터링 및 검색
- 연두색: 옵션
	- Preserve log: 새로운 페이지로 이동해도 해당 로그를 삭제하지 않음
	- Disable cache: 이미 캐시된 리소스도 서버에 요청함
- 노란색: 네트워크 로그
- 파란색: 네트워크 로그 요약 정보

원하는 항목을 선택하면, 해당 요청 및 응답 데이터를 확인할 수 있다. 

![[Pasted image 20251231021440.png]]

#### Network - Copy
로그를 우클릭하고, `Copy`에서 원하는 형태로 복사할 수 있다. 

![[Pasted image 20251231021727.png]]

`Copy as fetch`로 HTTP Request를 복사하고 `Console` 패널에 붙여넣기해서 그 Request를 실행하면, 같은 Request를 서버에 재전송할 수 있다!! 물론, 재전송이 그렇듯이, 딱히 안전한 행동은 아니다. 

![[Pasted image 20251231021845.png]]

>[!example] Console 코드 붙여넣기
>최신 브라우저의 경우, 임의의 코드를 콘솔에 붙여넣어서 실행하지 못 하도록 붙여넣기 기능을 막고 있다. 이런 경우, `allow pasting`을 입력하고 Enter를 눌러 붙여넣기 기능을 활성화한 뒤에야 복사한 내용을 붙여넣을 수 있다. 


### Application
---
쿠키, 캐시, 이미지, 폰트, 스타일시트 등 웹 애플리케이션과 관련한 리소스를 조회할 수 있다. 

![[Pasted image 20251231022201.png]]

Cookies에서는 브라우저에 저장된 쿠키 정보를 확인하고, 이를 수정할 수 있다. 

![[Pasted image 20251231022235.png]]


### Console Drawer
---
개발자 도구에 새로운 콘솔창을 열어서 가시성과 효율성을 높일 수 있다. Windows나 MacOS에서는 ESC 단축키로 네트워크 패널과 콘솔 패널을 동시에 사용할 수도 있다.

![[Pasted image 20251231022441.png]]


## 기타 브라우저 기능
---
### 페이지 소스 보기
---
**페이지 소스 보기**를 통해서 해당 웹 페이지와 관련한 소스 코드들을 전부 확인할 수 있다. Windows나 Linux에서는 Ctrl + U, MacOS에서는 Cmd + Opt + U 단축키로 이를 볼 수 있다. 

![[Pasted image 20251231022826.png]]

### Secret Browsing Mode
---
Chrome의 시크릿 모드에서는 새로운 브라우저 세션 = 새로운 창이 생성되며, 브라우저를 종료했을 때 방문 기록, 쿠키 및 사이트 데이터, 양식에 입력한 정보, 웹사이트에 부여한 권한이 저장되지 않는다. 

일반적으로 브라우저의 탭들은 쿠키를 공유하나, 시크릿 모드는 따로 창이 생성되어 쿠키를 공유하지 않는다. 이를 사용하면 같은 사이트를 여러 세션으로 사용할 수 있어 다수의 계정을 사용해 서비스를 점검할 때 유용하다. 

단축키는 Windows/Linux에서 Ctrl + Shift + N, MacOS에서 Cmd + Shift + N이다. 


## 관련 Dreamhack 워게임 문제
---
- [Carve Party](https://dreamhack.io/wargame/challenges/96/)
- [funjs](https://dreamhack.io/wargame/challenges/116/)
- [Secure Mail](https://dreamhack.io/wargame/challenges/92/)

지금까지 배웠던 내용들을 복습해 보면서, 워게임도 쉬운 거 여럿 풀면서 복습하자.
