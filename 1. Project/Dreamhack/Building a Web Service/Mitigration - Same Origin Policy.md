## 들어가며
---
[[Background - Cookie & Session]] 강의에서 Cookie에는 인증 상태를 나타내는 민감한 정보가 포함되며, 이는 브라우저 내부에 저장된다고 학습했다. 그리고 브라우저가 웹 서비스에 접속할 때 브라우저는 자동으로 쿠키를 헤더에 포함시켜 Request를 보낸다고 했다. 이 덕분에 여러 서비스에서 한 번만 로그인하면 일정 기간은 자동으로 로그인이 되어 서비스를 사용할 수 있다. 

하지만, 사용자가 악의적인 페이지를 접속했을 때, 페이지가 Javascript를 사용해 사용자의 SNS 웹 서비스로 Request를 보낸다면 어떻게 될까? 배운 대로라면, 브라우저는 Request를 보낼 때 헤더에 해당 웹 서비스의 쿠키를 포함할 것이다. 따라서 Javascript로 request를 보낸 페이지는 로그인 된 사용자의 SNS 응답을 받을 것이다. 이에 더해, 마음대로 사용자의 SNS 페이지에 글을 쓰거나, 삭제하고, SNS 메신저 내용을 읽는 것도 가능할 것이다. 

이와 같은 문제를 방지하기 위해 **동일 출처 정책, Same Origin Policy(SOP)** 보안 메커니즘이 탄생했다. SOP는 클라이언트 사이드의 웹 보안에 있어서 중요한 요소로, 실제로 클라이언트 사이드에서의 공격은 이 SOP를 우회하기 위해서라고 해도 과언이 아니다.

이번 강의에서는 SOP가 어떤 개념이고, 어떻게 동작하는지, 더 나아가 SOP에 대한 예외 상황에 대해서 알아볼 것이다.

![[Pasted image 20260103225404.png]]


## Same Origin Policy, SOP
---
### Same Origin Policy, SOP
---
브라우저는 인증 정보로 사용할 수 있는 쿠키를 브라우저의 내부에 보관하다, <u>웹 사용자가 웹 서비스에 접속할 때, 해당 웹 서비스에서 사용하는 인증 정보인 쿠키를 HTTP request에 포함시켜 전달한다.</u> 이와 같은 특징은 사이트에 직접 접속하는 것에만 한정되지 않고, 웹 리소스를 통해 간접적으로 타 사이트에 접근할 때에도 인증 정보인 쿠키를 함께 전송하는 특징을 가진다. 

이 특징 때문에 악의적인 페이지가 클라이언트의 권한을 이용해 대상 사이트에 HTTP request를 보내고, HTTP response 정보를 획득하는 코드를 실행할 수 있다. 이는 [[Exercise - Cookie & Session]]에서처럼 정보 유출 등의 보안 위협으로 이어질 수 있다. 따라서, 클라이언트의 입장에서는 가져온 데이터를 악의적인 페이지에서 읽을 수 없도록 해야 하며, 이것이 바로 브라우저의 보안 메커니즘인 **동일 출처 정책, Same Origin Policy (SOP)** 이다.


### SOP에서 Origin 구분 방법
---
그렇다면, 동일 출처 정책(SOP)에서 "동일 출처"는 어떻게 구분할까? 먼저, <u>Origin은 프로토콜(Protocol, Scheme), 포트(Port), 호스트(Host)로 구성</u>된다. 이는 곧 구성 요소가 모두 일치해야만 동일한 Origin이라 할 수 있으며, `https://same-origin.com/`이라는 origin과 아래의 URL을 비교했을 때의 결과는 다음과 같다. 

| URL                                      | 결과           | 이유         |
| ---------------------------------------- | ------------ | ---------- |
| https://same-origin.com/frame.html       | Same Origin  | Path만 다름   |
| http://same-origin.com/frame.html        | Cross Origin | Scheme이 다름 |
| https://cross.same-origin.com/frame.html | Cross Origin | Host가 다름   |
| https://same-origin.com:1234/            | Cross Origin | Port가 다름   |


### SOP 실습 - 브라우저
---
SOP는 Cross Origin이 아닌 Same Origin일 때만 정보를 읽을 수 있도록 동작한다. 예시로, 아래에 `https://dreamhack.io`에서 Javascript를 사용해 SOP를 테스트하는 코드를 작성해 보았다. 

새 창을 띄워서 https://dreamhack.io 로 이동해서 실습해 보자. 

#### Same Origin
```javascript
sameNewWindow = window.open('https://dreamhack.io/lecture');
console.log(sameNewWindow.location.href);
// 결과: https://dreamhack.io/lecture
```

#### Cross Origin
```javascript
crossNewWindow = window.open('https://theori.io');
console.log(crossNewWindow.location.href);
// 결과: Origin 오류 발생
```

여기서, `window.open`은 새 창을 띄우는 함수이며, `object.location.href`는 객체가 가리키고 있는 URL의 주소를 읽어오는 코드이다. 

#### Cross Origin 데이터 읽기/쓰기
위에서와 같이, 외부 출처에서 불러온 데이터를 읽으려고 할 때는 오류가 발생해 읽지 못하지만, 읽는 것 이외에 <u>데이터를 쓰는 것은 문제 없이 동작</u>한다. 따라서, 아래 코드는 잘 동작한다.

```javascript
crossNewWindow = window.open('https://theori.io');
crossNewWindow.location.href = "https://dreamhack.io";
```


### SOP 실습: Lab 환경
---
지금까지 배운 내용을 Same Origin Policy Lab을 통해 확인해 보자. 아래의 링크를 타고 Lab 실습환경을 열어서 학습하면 된다. 

[Lab으로 가기](https://learn.dreamhack.io/labs/135df378-f519-4deb-9276-8a03c043e605)


## Cross-Origin Resource Sharing (CORS)
---
### Same Origin Policy 제한 완화
---
SOP는 클라이언트의 웹 보안에서 중요한 요소이지만, 브라우저가 SOP에 구애 받지 않고 외부 출처에 대한 접근을 허용해주는 경우가 존재한다. 예를 들면, 이미지나 javascript, CSS 등의 리소스를 불러오는 `<img>`, `<style>`, `<script>` 등의 태그는 SOP의 영향을 받지 않는다. 

위의 경우들 외에도 웹 서비스에서 SOP를 완화해 다른 출처의 데이터를 처리해야 하는 경우도 존재한다. 예를 들어, 메이플스토리가 카페, 블로그, 로그인 서비스를 아래의 주소로 운영하고 있다고 하자. 서비스의 Host는 다르기 때문에, 브라우저는 각 사이트의 Origin이 다르다고 인식한다. 

- 카페: https://cafe.daum.net/maplestory
- 블로그: https://blog.maplestory.nexon.com/
- 로그인: http://m.maplestory.nexon.com/Authentication/Login
- 메인: https://maplestory.nexon.com/Home/Main

이러한 환경에서, 쌀숭이가 카페에 건의한 게시물을 메인에 올려 놓기 위해서 개발자는 메인 페이지에서 카페 서비스에 관련한 리소스를 요청하도록 해야 한다. 이때, 두 사이트는 각각 Origin이 다르기 때문에 SOP를 적용받지 않고 리소스를 공유할 방법이 필요하다. 

이 상황에서 자원을 공유하기 위해 사용할 수 있는 공유 방법을 **교차 출처 리소스 공유 (Cross-Origin Resource Sharing, CORS)** 라고 한다. Cross Origin의 자원을 공유하는 방법은 CORS와 관련한 HTTP 헤더를 추가해 전송하는 방법을 사용한다. 이 외에도 JSON with Padding (JSONP) 방법을 통해 CORS를 대체할 수 있다. 


### Cross-Origin Resource Sharing, CORS
---
CORS는 HTTP 헤더에 기반한 방법으로, 발신 측에서 CORS 헤더를 설정해서 Request를 보내면, 수신측에서 헤더를 구분해 정해진 규칙에 맞게 데이터를 가져갈 수 있도록 설정한다. 

아래 코드는 각각 웹 리소스를 요청하는 발신측 코드의 일부와 발신측의 HTTP request이다. 

```javascript
/*
    XMLHttpRequest 객체를 생성합니다. 
    XMLHttpRequest는 웹 브라우저와 웹 서버 간에 데이터 전송을
    도와주는 객체 입니다. 이를 통해 HTTP 요청을 보낼 수 있습니다.
*/
xhr = new XMLHttpRequest();
/* https://theori.io/whoami 페이지에 POST 요청을 보내도록 합니다. */
xhr.open('POST', 'https://theori.io/whoami');
/* HTTP 요청을 보낼 때, 쿠키 정보도 함께 사용하도록 해줍니다. */
xhr.withCredentials = true;
/* HTTP Body를 JSON 형태로 보낼 것이라고 수신측에 알려줍니다. */
xhr.setRequestHeader('Content-Type', 'application/json');
/* xhr 객체를 통해 HTTP 요청을 실행합니다. */
xhr.send("{'data':'WhoAmI'}");
```

```javascript
OPTIONS /whoami HTTP/1.1
Host: theori.io
Connection: keep-alive
Access-Control-Request-Method: POST
Access-Control-Request-Headers: content-type
Origin: https://dreamhack.io
Accept: */*
Referer: https://dreamhack.io/
```

표를 살펴보면, 발신측에서 POST 방식으로 HTTP request를 보냈으나, OPTIONS 메소드를 가진 HTTP request가 전달된 것을 확인할 수 있다. 이를 <u>CORS preflight</u>라고 하며, 수신측에 웹 리소스를 요청해도 되는지를 물어보는 과정이다. 

위 HTTP request를 살펴보면, "Access-Control-Request"로 시작하는 헤더가 존재한다. 해당 헤더 뒤의 **Method**와 **Header**는 각각 메소드와 헤더를 추가적으로 사용할 수 있는지 물어본다. 

이처럼 물어보면, 서버는 다음과 같이 HTTP response를 보낸다. 

```javascript
HTTP/1.1 200 OK
Access-Control-Allow-Origin: https://dreamhack.io
Access-Control-Allow-Methods: POST, GET, OPTIONS
Access-Control-Allow-Credentials: true
Access-Control-Allow-Headers: Content-Type
```

다음은 이 결과에 대한 설명이다. 

| Header                           | 설명                                    |
| -------------------------------- | ------------------------------------- |
| Access-Control-Allow-Origin      | 이 값에 해당하는 Origin에서 들어온 request만 처리한다. |
| Access-Control-Allow-Methods     | 이 값에 해당하는 메소드의 요청만 처리한다.              |
| Access-Control-Allow-Credentials | 쿠키 사용 여부를 결정한다.                       |
| Access-Control-Allow-Headers     | 이 값에 해당하는 헤더의 사용 여부를 나타낸다.            |
위 과정을 마치면, 브라우저는 수신측의 응답이 발신측의 요청과 맞는지 검사하고, 그때 비로소 POST request를 보내 수신측의 웹 리소스를 요청하는 HTTP request를 보낸다.


## JSON with Padding (JSONP)
---
JSONP 방식은 이미지나 Javascript, CSS 등의 리소스는 SOP에 구애 받지 않고 외부 출처에 대해 접근을 허용한다는 점을 이용해 `<script>` 태그로 Cross Origin의 데이터를 불러 온다. 단, `<script>` 태그 내에서는 해당 데이터를 Javascript의 코드 그 자체로 인식하기 때문에, **Callback** 함수를 사용해야 한다.

Cross Origin에 데이터를 요청할 때 callback 파라미터에 어떤 함수로 받아오는 데이터를 핸들링할지 넘겨주면, 대상 서버는 전달된 callback으로 데이터를 감싸 응답한다. 

예시 코드를 보면, 13번째 줄에서 Cross Origin의 데이터를 불러오며, 이 때 callback 파라미터로 `myCallback`을 함께 전달한다. Cross Origin에서는 응답할 데이터를 `myCallback` 함수의 인자로 전달할 수 있도록 `myCallback`으로 감싸 Javascript 코드를 반환한다. 반환된 코드는 요청한 측에서 실행되기 때문에 3~6번 줄에서 정의된 `myCallback` 함수로 전달된 데이터를 읽을 수 있다. 

다음은 각각 웹 리소스 요청 코드와 웹 리소스 요청에 따른 응답 코드이다. 

```javascript
<script>
/* myCallback이라는 콜백 함수를 지정합니다. */
function myCallback(data){
    /* 전달받은 인자에서 id를 콘솔에 출력합니다.*/
	console.log(data.id)
}
</script>
<!--
https://theori.io의 스크립트를 로드하는 HTML 코드입니다.
단, callback이라는 이름의 파라미터를 myCallback으로 지정함으로써
수신측에게 myCallback 함수를 사용해 수신받겠다고 알립니다.
-->
<script src='http://theori.io/whoami?callback=myCallback'></script>
```

```javascript
/*
수신측은 myCallback 이라는 함수를 통해 요청측에 데이터를 전달합니다.
전달할 데이터는 현재 theori.io에서 클라이언트가 사용 중인 계정 정보인
{'id': 'dreamhack'} 입니다. 
*/
myCallback({'id':'dreamhack'});
```

단, JSONP는 CORS가 생기기 전에 사용하던 방법으로, 일종의 편법이기 때문에, 새롭게 코드를 작성할 때는 CORS를 사용해야 한다. 
