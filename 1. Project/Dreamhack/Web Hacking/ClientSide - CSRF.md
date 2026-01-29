## 들어가며
---
사용자의 신원 정보가 포함된 쿠키는 일종의 서명과 같은 역할을 한다. 이때 서명이란, 서명자가 문서의 내용에 동의한다는 것을 의미하기 때문에 사용자의 식별 정보가 포함된 쿠키 또한 클라이언트에서 보내진 요청이 사용자로부터 왔으며, 사용자가 동의했고, 따라서 요청에 사용자의 권한이 같이 있음을 의미한다. 실생활과 밀접한 웹 서비스가 많아지고 있기 때문에, 서명을 신중하게 관리하는 것만큼 중요한 웹 사이트의 쿠키 또한 잘 보관해야 한다. 

서명과 관련한 범죄는 서명을 날조하거나, 서명된 문서의 내용을 위조하는 것 등이 있으며, 이런 범죄는 웹 해킹의 공격 기법들과도 대응된다. **전자는 XSS를 통해서 쿠키를 탈취하는 공격과 비슷하고, 후자는 이번 강의에서 배울 교차 사이트 요청 위조 (Cross Site Request Forgery, CSRF)와 비슷하다.**

CSRF는 사용자를 속여서 의도하지 않은 요청에 동의하게 하는 공격으로, 그럴듯한 웹 페이지를 만들어서 사용자의 입력을 유도하고, 사용자가 값을 입력하면 이를 은행이나 중요 웹 사이트에 전송해서 마치 사용자가 동의한 것 같은 요청을 발생시킨다. 만약 사용자가 "자동 로그인"등의 기능을 사용해서 브라우저에 세션 쿠키를 저장하고 있었다면, 계좌 이체가 이뤄지거나 비밀번호 초기화가 이뤄질 수 있다. 


## Cross Site Request Forgery (CSRF)
---
웹 서비스는 쿠키 또는 세션으로 사용자를 식별한다. 이때 임의의 사용자의 쿠키를 식별할 수 있다면, 이는 곧 임의 사용자의 권한으로 웹 서비스의 기능을 사용할 수 있다. CSRF는 임의 사용자의 권한으로 임의 주소에 HTTP request를 보낼 수 있는 취약점으로, 공격자는 임의 사용자의 권한으로 서비스 기능을 사용해 이득을 취할 수 있다.

예를 들어, 사용자의 계정으로 임의의 금액을 송금해 금전적인 이득을 취하거나 비밀번호를 변경해 계정을 탈취하고, 관리자 계정을 공격해 공지사항 작성 등으로 혼란을 불러올 수도 있다. 

아래 코드는 송금 기능을 수행하는 엔드포인트 코드의 예시로, CSRF 취약점이 존재한다.

**송금 기능을 수행하는 코드**
```python
# 사용자가 /sendmoney에 접속했을 때 아래의 송금 기능을 웹 서비스가 실행
@app.route('/sendmoney')
def sendmoney(name):
	# 송금을 받는 사람, 금액을 입력
	to_user = request.args.get('to')
	amount = int(request.args.get('amount'))
	
	# 송금 기능 실행 후 결과 반환
	success_status = send_money(to_user, amount)
	
	# 송금이 성공했을 때, 실패했을 때 처리
	if success_status:
		return "Send success."
	else:
		return "Send fail."
```

**사용자의 송금 요청**
```http
GET /sendmoney?to=kookmin&amount=1337 HTTP/1.1
Host: bank.kookmin.ac
Cookie: session=IeheighaiToo4eenahw3
```

코드를 살펴보면, 사용자로부터 예금주와 금액을 입력받아 송금을 수행하며, 계좌 비밀번호, OTP 등을 사용하지 않기 때문에 로그인한 사용자는 별도의 인증 절차 없이 해당 기능을 쓸 수 있다. 


### Cross Site Request Forgery 동작
---
CSRF 공격에 성공하기 위해서는 공격자가 작성한 악성 스크립트를 사용자가 실행해야 한다. 이는 공격자가 사용자에게 메일을 보내거나 게시판에 글을 작성해 사용자가 글을 열어보도록 유도하는 방법이 있다. (여기서 말하는 악성 스크립트란 HTTP request를 보내는 코드이다)

CSRF 공격 스크립트는 HTML 또는 JavaScript를 통해 작성할 수 있다. 아래 사진 및 코드는 HTML으로 작성한 스크립트의 예시로, 이미지를 불러오는 `<img>` 태그를 사용하거나 웹 페이지에 입력된 양식을 전송하는 `<form>` 태그 등을 사용해 HTTP request를 보내면 HTTP 헤더인 Cookie에 사용자의 인증 정보가 포함되어 전송된다. 

![[Pasted image 20260128231400.png]]
**`<img>` 태그를 사용한 스크립트 예시**
해당 태그는 이미지의 크기를 줄일 수 있는 옵션을 제공하며, 이를 활용하면 사용자에게 들키지 않고 임의의 페이지에 요청을 보낼 수 있다. (*크기 조정 옵션은 필수가 아니기는 하다*)

```html
<img src='http://bank.dreamhack.io/sendmoney?to=Dreamhack&amount=1337' width=0px height=0px>
```

**JavaScript를 사용한 스크립트 예시**
새로운 창을 띄우고, 현재 창의 주소를 옮기는 등의 행위가 가능하다. 

```javascript
/* 새 창 띄우기 */
window.open('http://bank.dreamhack.io/sendmoney?to=Dreamhack&amount=1337');

/* 현재 창 주소 옮기기 */
location.href = 'http://bank.dreamhack.io/sendmoney?to=Dreamhack&amount=1337');
location.replace('http://bank.dreamhack.io/sendmoney?to=Dreamhack&amount=1337');
```


### CSRF 실습
---
지금까지 배운 내용을 Lab을 통해 학습하며, [이 링크](https://learn.dreamhack.io/labs/09df251d-858e-4439-a6d8-2028f7fbc783)를 통해서 Lab 실습 환경을 사용할 수 있다.


### XSS와 CSRF의 차이
---
XSS와 CSRF는 스크립트를 웹 페이지에 작성해 공격한다는 점에서 매우 유사하다. 

**공통점**
- 클라이언트를 대상으로 수행하는 공격
- 사용자가 악성 스크립트가 포함된 페이지에 접속하도록 유도해야 함

**차이점**
- XSS
	- 인증 정보인 쿠키 및 세션 탈취를 목적으로 하는 공격
	- 공격할 사이트의 오리진에서 스크립트를 실행
- CSRF
	- 사용자가 임의 페이지에 HTTP request를 보내도록 유도하는 공격
	- 공격자는 악성 스크립트가 포함된 페이지에 접근한 사용자의 권한으로 임의 기능 실행


## 마치며
---
이번 강의에서는 CSRF의 개념, 공격 방식에 대해 알아보았다. 지금까지 배웠던 내용들을 Dreamhack에서 제공하는 워게임이나 CTF 등으로 복습해보며 잘 숙지하는 것이 중요하다. 