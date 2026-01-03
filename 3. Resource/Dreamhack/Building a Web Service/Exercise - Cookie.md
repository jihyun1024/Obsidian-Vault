## 들어가며
---
### 서론
---
이번 강의에서는 [[Exercise - devtools-sources]]처럼, 별도의 풀이 노트 없이, 잘못된 쿠키의 설계로 발생할 수 있는 문제점을 다루는 [cookie](https://dreamhack.io/wargame/challenges/6/) 문제를 풀어본다. 


### 배경 지식
---
본 문제의 서버는 Python의 Flask 프레임워크를 통해 구현되었다. 웹 프레임워크란,. 웹 페이지를 개발하는 과정에서 겪는 유지보수, 반복 작업 등의 여러 귀찮은 작업들을 줄이기 위해 명명 및 스크립트 작성 규칙, 디렉터리 구조 등이 명시된 개발 표준 가이드를 정의한 것으로, 개발자가 이에 맞춰 편하게 개발할 수 있도록 한다. 

Python을 기반으로 개발된 문제이기 때문에 기초적인 Python 사용법과 문법을 알고 있어야 한다. 본 강의에서는 문제를 쉽게 이해할 수 있도록 각 부분마다 주석을 달아 놓았다. 


### 문제 목표 및 기능 요약
---
이 문제의 목표는 관리자 권한을 획득해 플래그를 획득하는 것이다. 문제에서는 다음 두 페이지를 제공한다. 이때, 각각의 페이지는 주소창 옆에 `/`나 `/login`을 덧붙이면 갈 수 있다. 

- `/`: 이용자의 `username`을 출력하고 관리자 계정인지 확인
- `/login`: `username`, `password`를 입력받아 로그인


## 웹 서비스 분석
---
### 엔드포인트: /
---
아래 코드는 index page를 구성하는 코드이다. 해당 페이지에서는 요청에 포함된 쿠키를 통해 사용자를 식별하며, 만약 cookie에 존재하는 `username`이 "admin"일 경우, 플래그를 출력한다. 

```python
@app.route('/') # / 페이지 라우팅 
def index():
    username = request.cookies.get('username', None) # 이용자가 전송한 쿠키의 username 입력값을 가져옴
    if username: # username 입력값이 존재하는 경우
        return render_template('index.html', text=f'Hello {username}, {"flag is " + FLAG if username == "admin" else "you are not admin"}') # "admin"인 경우 FLAG 출력, 아닌 경우 "you are not admin" 출력
    return render_template('index.html')
```


### 엔드포인트: /login
---
아래 코드는 로그인 페이지를 구성하는 코드이다. 

```python
@app.route('/login', methods=['GET', 'POST']) # login 페이지 라우팅, GET/POST 메소드로 접근 가능
def login():
    if request.method == 'GET': # GET 메소드로 요청 시
        return render_template('login.html') # login.html 페이지 출력
    elif request.method == 'POST': # POST 메소드로 요청 시
        username = request.form.get('username') # 이용자가 전송한 username 입력값을 가져옴
        password = request.form.get('password') # 이용자가 전송한 password 입력값을 가져옴
        try:
            pw = users[username] # users 변수에서 이용자가 전송한 username이 존재하는지 확인
        except: 
            return '<script>alert("not found user");history.go(-1);</script>' # 존재하지 않는 username인 경우 경고 출력
        if pw == password: # password 체크
            resp = make_response(redirect(url_for('index')) ) # index 페이지로 이동하는 응답 생성
            resp.set_cookie('username', username) # username 쿠키 설정
            return resp 
        return '<script>alert("wrong password");history.go(-1);</script>' # password가 동일하지 않은 경우
```

**GET**
`username`과 `password`를 입력해 로그인을 할 수 있는 페이지를 제공한다. 

**POST**
사용자가 입력한 `username`과 `password`를 받아 `users` 변수와 비교한다. 아래 코드는 `users` 변수가 선언된 코드로, `guest` 계정의 비밀번호는 "guest", `admin` 계정의 비밀번호는 파일에서 읽어온 플래그라는 것을 알 수 있다. 

```python
try:
    FLAG = open('./flag.txt', 'r').read() # flag.txt 파일로부터 FLAG 데이터를 가져옴.
except:
    FLAG = '[**FLAG**]'
users = {
    'guest': 'guest',
    'admin': FLAG # FLAG 데이터를 패스워드로 선언
}
```


## 취약점 분석
---
아래 코드를 살펴보면, 사용자의 계정을 나타내는 `username` 변수가 요청에 포함된 쿠키에 의해 결정되며, 서버는 별다른 인증 없이 사용자의 요청에 포함된 쿠키를 신뢰해 사용자를 식별하기 때문에 문제가 발생한다. 

이는 곧 임의로 쿠키를 조작해도 아무런 인증 절차 없이 타 계정 정보를 삽입해 계정을 탈취할 수 있다는 것과 동일하다. 

```python
@app.route('/') # / 페이지 라우팅 
def index():
    username = request.cookies.get('username', None) # 이용자가 전송한 쿠키의 username 입력값을 가져옴
    if username: # username 입력값이 존재하는 경우
        return render_template('index.html', text=f'Hello {username}, {"flag is " + FLAG if username == "admin" else "you are not admin"}') # "admin"인 경우 FLAG 출력, 아닌 경우 "you are not admin" 출력
    return render_template('index.html')
```


## 익스플로잇
---
본 문제를 해결하기 위해서는 쿠키에 존재하는 `username`을 "admin"으로 조작해야 한다. 아래 사진처럼, 웹 브라우저의 개발자 도구를 활용해 Application 탭에 들어가 Cookies 탭에서 Name과 Value를 각각 `username`과 "admin"으로 설정해 서버에 요청하면 플래그를 획득할 수 있다. 

![[Pasted image 20260103205600.png]]

![[Pasted image 20260103205606.png]]
