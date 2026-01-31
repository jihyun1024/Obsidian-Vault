## 들어가며
---
이번 시간에는 CSRF 취약점이 존재하는 워게임인 [csrf-2](https://dreamhack.io/wargame/challenges/269)를 풀어본다. 해당 문제는 Python Flask 프레임워크를 통해 구현되었으며, CSRF를 통해 관리자의 기능을 사용해야 하기 때문에 관리자로 로그인하는 시나리오가 필요하여 Selenium을 사용했다. 


### 문제 목표 및 기능 요약
---
csrf-2 문제의 목표는 CSRF를 통해 관리자 계정으로 로그인하는 것이다.

```
users = {
    'guest': 'guest',
    'admin': FLAG
}
```

문제에서는 다음 네 개의 페이지를 제공한다. 

- `/` - 인덱스 페이지
- `/vuln` - 사용자가 입력한 값을 출력하며, XSS가 발생할 수 있는 키워드는 필터링됨
- `/flag` - GET, POST 요청을 처리하며, CSRF 공격 방어와 세션 관리를 수행
- `/login` - 로그인 페이지를 처리
- `/change_password` - 비밀번호 변경을 처리하며, 사용자의 세션을 확인한 후 비밀번호를 새롭게 설정


## 엔드포인트 분석
---
아래 코드는 문제 페이지에서 받을 수 있는 코드 중 *app.py*에 해당하는 코드이다. 

```python
#!/usr/bin/python3
from flask import Flask, request, render_template, make_response, redirect, url_for
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
import urllib
import os
  
app = Flask(__name__)
app.secret_key = os.urandom(32)
  
try:
    FLAG = open("./flag.txt", "r").read()
except:
    FLAG = "[**FLAG**]"
  
users = {
    'guest': 'guest',
    'admin': FLAG
}
  
session_storage = {}
  
def read_url(url, cookie={"name": "name", "value": "value"}):
    cookie.update({"domain": "127.0.0.1"})
    try:
        service = Service(executable_path="/chromedriver")
        options = webdriver.ChromeOptions()
        for _ in [
            "headless",
            "window-size=1920x1080",
            "disable-gpu",
            "no-sandbox",
            "disable-dev-shm-usage",
        ]:
            options.add_argument(_)
        driver = webdriver.Chrome(service=service, options=options)
        driver.implicitly_wait(3)
        driver.set_page_load_timeout(3)
        driver.get("http://127.0.0.1:8000/")
        driver.add_cookie(cookie)
        driver.get(url)
    except Exception as e:
        driver.quit()
        print(str(e))
        # return str(e)
        return False
    driver.quit()
    return True
  
  
def check_csrf(param, cookie={"name": "name", "value": "value"}):
    url = f"http://127.0.0.1:8000/vuln?param={urllib.parse.quote(param)}"
    return read_url(url, cookie)
  
  
@app.route("/")
def index():
    session_id = request.cookies.get('sessionid', None)
    try:
        username = session_storage[session_id]
    except KeyError:
        return render_template('index.html', text='please login')
  
    return render_template('index.html', text=f'Hello {username}, {"flag is " + FLAG if username == "admin" else "you are not an admin"}')
  
  
@app.route("/vuln")
def vuln():
    param = request.args.get("param", "").lower()
    xss_filter = ["frame", "script", "on"]
    for _ in xss_filter:
        param = param.replace(_, "*")
    return param
  
  
@app.route("/flag", methods=["GET", "POST"])
def flag():
    if request.method == "GET":
        return render_template("flag.html")
    elif request.method == "POST":
        param = request.form.get("param", "")
        session_id = os.urandom(16).hex()
        session_storage[session_id] = 'admin'
        if not check_csrf(param, {"name":"sessionid", "value": session_id}):
            return '<script>alert("wrong??");history.go(-1);</script>'
  
        return '<script>alert("good");history.go(-1);</script>'
  
  
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'GET':
        return render_template('login.html')
    elif request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')
        try:
            pw = users[username]
        except:
            return '<script>alert("not found user");history.go(-1);</script>'
        if pw == password:
            resp = make_response(redirect(url_for('index')) )
            session_id = os.urandom(8).hex()
            session_storage[session_id] = username
            resp.set_cookie('sessionid', session_id)
            return resp
        return '<script>alert("wrong password");history.go(-1);</script>'
  
  
@app.route("/change_password")
def change_password():
    pw = request.args.get("pw", "")
    session_id = request.cookies.get('sessionid', None)
    try:
        username = session_storage[session_id]
    except KeyError:
        return render_template('index.html', text='please login')
  
    users[username] = pw
    return 'Done'
  

app.run(host="0.0.0.0", port=8000)
```

이때 `/vuln` 엔드포인트를 구성하는 함수인 `vuln` 함수를 보면, 사용자가 전달한 `param` 파라미터의 값을 출력하는데, 이때 사용자의 파라미터에 "`frame`", "`script`", "`on`" 이 세 가지의 악성 키워드가 포함되어 있으면 이를 "`*`" 문자로 치환한다.

그 외에도, `/flag` 엔드포인트의 POST 메서드에서는 `param` 파라미터 값을 가져온 뒤, `session_id`를 생성하고, 생성한 `session_id`를 키로 사용하여 `admin` 값을 `session_storage` 딕셔너리에 저장한다. 이후 `check_csrf` 함수에서 `session_id`가 유효한지 확인하고 적절한 경고 메시지를 반환한다. 

`/change_password` 페이지에서는 GET request를 통해 `pw` 매개변수의 값을 가져와 `pw` 변수에 저장하며, 브라우저 쿠키에서 `"session_id"` 값을 가져와 `session_id` 변수에 저장한다. 이때 `session_id`를 사용해 로그인한 사용자를 확인하는데, `session_id`가 세션 저장소에 없다면 `"please login"` 메시지와 함께 `index.html` 템플릿을 렌더링하여 보여주며, 있다면 `users[username]=pw` 세션을 통해 확인된 사용자의 비밀번호를 새로운 비밀번호로 변경한다. 


## 취약점 분석
---
이 문제도 저번 [[Exercise - CSRF]] 문제와 마찬가지로, `/vuln` 기능에서 취약점이 발생하는데 입력값에서 `frame`, `script`, `on` 세 가지의 키워드를 필터링하기 때문에 XSS 공격은 통하지 않지만, 필터링 키워드 이외의 `<`, `>` 문자나 다른 태그들은 사용할 수 있어 CSRF 공격은 통한다.

```python
@app.route("/vuln")
def vuln():
    param = request.args.get("param", "").lower()
    xss_filter = ["frame", "script", "on"]
    for _ in xss_filter:
        param = param.replace(_, "*")
    return param
```


## 익스플로잇
---
이 문제는 `/flag` 페이지에서 공격이 가능하다. `/flag` 페이지에서 `admin`의 `session_id`가 저장되기 때문에 공격 코드가 삽입된 `/flag` 페이지에서 `admin`의 비밀번호를 변경하기 위해서는 `/change_password` 페이지를 접근해야 하며, 이를 위해 CSRF 공격으로 `/flag` 페이지를 방문하는 사용자가 `/change_password` 페이지로 요청을 전송하도록 공격 코드를 작성해야 한다. 


### CSRF 공격 코드 작성 및 실행
---
`/flag` 페이지에서 사용자가 `/change_password` 페이지를 통해 `admin`의 비밀번호를 바꾸어야 하기 때문에, 다음의 조건을 갖춰 공격 코드를 작성해야 한다. 

1. CSRF 공격을 실행할 수 있는 태그를 사용해야 한다. 
2. `/change_password` 페이지로 이동할 수 있어야 한다. 
3. 해당 페이지에서 `pw` 파라미터를 사용해 원하는 비밀번호로 admin의 비밀번호를 변경한다.

따라서, 다음과 같은 공격 코드 작성이 가능하다. 

```html
<!-- 변경할 비밀번호는 admin 대신 아무 거나 가능하다 -->
<img src='/change_password?pw=admin'>
```

공격 코드를 작성했다면, `/flag` 페이지의 입력 폼에 공격 코드를 넣어 전송하여 플래그를 메인 화면에서 획득할 수 있다. 


## 마치며
---
이번 강의에서는 csrf-2 문제를 통해 [[ClientSide - CSRF]] 강의에서 배운 CSRF 취약점을 실습했다. 일반 유저가 아닌 admin으로 로그인을 우회하기 위해 admin의 비밀번호를 바꾸는 기능을 admin의 의도와는 무관하게 실행하는 것이 이번 문제의 핵심이다. 

CSRF 공격은 이번 문제에서 다뤄본 결과 외에도, 게시판에서 공격자가 특정한 글을 공지사항으로 올리거나 삭제하는 등의 기능을 실행할 수 있으며, 공격자의 계정을 게시판 관리자로 승격하는 등의 행위로도 응용하여 사용할 수 있다.