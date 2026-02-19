## 들어가며
이번 강의에서는 CSRF 취약점이 존재하는 [csrf-1](https://dreamhack.io/wargame/challenges/26/)을 풀어보면서 관리자만 수행할 수 있는 메모 작성 기능을 이용해 플래그를 획득하는 실습을 진행한다. 

---
### 배경 지식
이 문제는 Python Flask 프레임워크를 통해 구현되었으며, CSRF를 통해 관리자의 기능을 이용해야 하기 때문에 관리자가 방문하는 시나리오가 필요하여 Selenium을 사용했다. 

---
### 문제 목표 및 기능 요약
csrf-1 문제의 목표는 CSRF를 통해 관리자 계정으로 특정한 기능을 실행하는 것으로, 문제에서는 다음 네 페이지를 제공한다. 

- `/` - 인덱스 페이지
- `/vuln` - 사용자가 입력한 값을 출력, XSS가 발생할 수 있는 키워드는 필터링
- `/memo` - 사용자가 메모를 남길 수 있으며, 작성한 메모를 출력
- `/admin/notice_flag` - 메모에 FLAG를 작성하는 기능으로, 로컬호스트에서 사이트 관리자만 접속 가능
- `/flag` - 전달된 URL에 임의의 사용자가 접속하게끔 함

---
## 웹 서비스 분석
아래 코드는 문제 페이지에서 받을 수 있는 코드 중 *app.py*에 해당하는 코드이다. 

```python
#!/usr/bin/python3
from flask import Flask, request, render_template
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
    return render_template("index.html")
  
  
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
        if not check_csrf(param):
            return '<script>alert("wrong??");history.go(-1);</script>'
  
        return '<script>alert("good");history.go(-1);</script>'
  
  
memo_text = ""
  
  
@app.route("/memo")
def memo():
    global memo_text
    text = request.args.get("memo", None)
    if text:
        memo_text += text
    return render_template("memo.html", memo=memo_text)
  
  
@app.route("/admin/notice_flag")
def admin_notice_flag():
    global memo_text
    if request.remote_addr != "127.0.0.1":
        return "Access Denied"
    if request.args.get("userid", "") != "admin":
        return "Access Denied 2"
    memo_text += f"[Notice] flag is {FLAG}\n"
    return "Ok"
  
  
app.run(host="0.0.0.0", port=8000)
```

이때 `/vuln` 엔드포인트를 구성하는 함수인 `vuln` 함수를 보면, 사용자가 전달한 `param` 파라미터의 값을 출력하는데, 이때 사용자의 파라미터에 "`frame`", "`script`", "`on`" 이 세 가지의 악성 키워드가 포함되어 있으면 이를 "`*`" 문자로 치환한다. 

이러한 키워드 필터링은 XSS 공격을 방지하기 위한 목적으로 넣은 것으로, 본 문제의 의도는 XSS 공격을 사용하지 않고 대신 CSRF 공격으로 관리자의 기능을 수행하는 것이 목적임을 알 수 있다. 이는 `admin_notice_flag` 함수를 봐도 나와 있다. 

`/admin/notice_flag` 페이지 자체는 모두가 접근할 수 있지만, 일반 유저가 해당 페이지에 접근할 때의 IP주소는 조작할 수 없다. 따라서 일반 유저의 IP가 자신의 컴퓨터를 의미하는 로컬호스트 IP가 되는 것은 불가능하기 때문에, 단순히 접근하는 것 만으로는 플래그를 획득할 수 없다. 

>[!example] 로컬호스트(Localhost)
><u>로컬호스트는 컴퓨터 네트워크에서 사용하는 호스트명으로, **자기 자신의 컴퓨터**를 의미한다.</u> 로컬호스트를 IPv4 방식으로 표현했을 때는 `127.0.0.1`, IPv6로 표현했을 때는 `00:00:00:00:00:00:00:01`로 표현한다. 

---
## 취약점 분석
`/vuln` 기능은 사용자의 입력 값을 페이지에 출력하고, 입력 값에서 `frame`, `script`, `on` 세 가지의 키워드를 필터링하기 때문에 XSS 공격은 불가능하다.

그러나, 필터링 키워드 이외의 꺽쇠(`<`, `>`)나 다른 키워드나 태그 등은 필터링하지 않기 때문에 CSRF 공격을 사용할 수 있다. 

---
## 익스플로잇
이 문제에서는 `/vuln` 페이지에서 CSRF 공격이 가능하다. 따라서 공격 코드가 삽입된 `/vuln` 페이지를 다른 사용자가 방문할 경우, 의도하지 않은 페이지로 요청을 전송하는 시나리오의 익스플로잇을 구상해야 한다. 

플래그를 얻기 위해서는 `/admin/notice_flag` 페이지를 로컬호스트에서 접근해야 하며, 이를 위해 CSRF 공격으로 `/vuln` 페이지를 방문하는 로컬호스트 사용자, 즉 admin이 `/admin/notice_flag` 페이지로 요청을 전송하도록 공격 코드를 작성해야 한다. 

이를 위해서는 로컬호스트 환경의 사용자가 임의 페이지를 방문하게 해야 하고, 이때 `/flag` 페이지를 이용해야 한다. 

---
### 테스트베드 생성
먼저 CSRF 취약점 발생 여부를 확인하기 위해, HTTP 응답을 받을 수 있는 웹 서버가 필요하다. 만약 외부에서 접근 가능한 웹 서버가 없다면, Dreamhack에서 제공하는 [Dreamhack Tools Service](https://tools.dreamhack.games/)를 사용하면 된다. 

해당 서비스에서 제공하는 `Request Bin` 기능은 랜덤한 URL을 제공하고 제공된 URL에 대해 사용자의 접속 기록을 저장하기 때문에 XSS, CSRF 공격을 테스트하기 좋다. 

![[Pasted image 20260129023406.png]]

---
### CSRF 취약점 테스트
테스트베드를 생성했다면, CSRF 공격 코드를 작성하고 취약점 발생 여부를 확인한다. [[ClientSide - CSRF]]에서 배운 `<img>` 태그를 사용해 테스트베드 URL에 접속하는 공격 코드를 다음과 같이 작성한 뒤, 취약점이 발생하는 페이지의 `param`에 이 코드를 삽입한다. 

```html
<img src='https://jugwwka.request.dreamhack.games'>
```

아래 사진을 보면 공격 코드에 의해 이미지가 화면에 출력되었으며, 생성된 테스트베드에 요청이 온 것을 확인할 수 있다. 

![[Pasted image 20260129023756.png]]

---
### CSRF 공격 코드 작성 및 실행
로컬호스트에 있는 사용자가 `/admin/notice_flag` 페이지를 방문하도록 해야 하기 때문에 아래와 같이 공격 코드를 작성해야 한다. 이때, `userid` 파라미터가 `admin`인지 검사하는 부분이 있기 때문에 해당 문자열까지도 포함해야 한다. 

```html
<img src='/admin/notice_flag?userid=admin'>
```

공격 코드를 작성했다면, `flag` 페이지에서 해당 공격 코드를 전송한다. 

![[Pasted image 20260129024058.png]]

성공적으로 전송했다면 로컬호스트에서 `http://127.0.0.1:8000/vuln?param=<img src='/admin/notice_flag?userid=admin'>`에 접속하게 되며, 아래 사진과 같이 `memo` 페이지에 들어가서 앞서 수행한 CSRF 공격으로 관리자가 `/admin/notice_flag` 페이지에 방문한 것을 확인할 수 있다. 

![[Pasted image 20260129024251.png]]

---
## 마치며
이번 강의에서는 csrf-1 문제를 통해 CSRF 취약점을 실습해 봤다. 로컬호스트 유저만 실행할 수 있는 기능을 해당 유저의 의도와는 무관하게 실행하는 것이 이 문제의 핵심 원리이다.

CSRF 공격은 이번 문제 외에도, 게시판 서비스에서 공격자가 특정 글을 공지사항으로 업로드하거나 삭제하는 등의 기능을 실행할 수도 있으며 심지어는 공격자의 계정을 admin으로 승격시키는 등의 행위도 할 수 있다. 