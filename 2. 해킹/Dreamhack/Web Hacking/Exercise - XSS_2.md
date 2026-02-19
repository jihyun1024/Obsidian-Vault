## 들어가며
이번 강의에서는 XSS 취약점이 존재하는 또 다른 문제인 [xss-2](https://dreamhack.io/wargame/challenges/268)를 풀어보면서 다른 사용자의 클라이언트에 저장된 쿠키를 탈취하는 실습을 진행한다. 

이번 문제는 Python Flask 프레임워크를 통해 구현되었으며, XSS를 통해 다른 사용자의 쿠키를 탈취해야 하는 시나리오이기 때문에 [[Exercise - XSS]]처럼 Selenium 패키지를 사용했다. 

---
### 문제 목표 및 기능 요약
xss-2 문제의 목표는 XSS를 통해 임의 사용자의 쿠키를 탈취하는 것이며, 문제에서는 저번 xss-1 문제처럼 네 개의 페이지를 제공한다. 

- `/`: 인덱스 페이지
- `/vuln`: 사용자가 입력한 값을 출력
- `/memo`: 사용자가 메모를 남길 수 있으며, 작성한 메모를 출력
- `/flag`: 사용자가 전달한 URL에 임의의 사용자가 접근하도록 하며, **해당 사용자의 쿠키에는 Flag가 존재**

---
## 웹 서비스 분석
아래 코드는 문제 페이지에서 다운로드 받을 수 있는 코드 중 *app.py*에 해당하는 코드이다. 

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
        # return str(e)
        return False
    driver.quit()
    return True


def check_xss(param, cookie={"name": "name", "value": "value"}):
    url = f"http://127.0.0.1:8000/vuln?param={urllib.parse.quote(param)}"
    return read_url(url, cookie)
  

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/vuln")
def vuln():
    return render_template("vuln.html")
  

@app.route("/flag", methods=["GET", "POST"])
def flag():
    if request.method == "GET":
        return render_template("flag.html")
    elif request.method == "POST":
        param = request.form.get("param")
        if not check_xss(param, {"name": "flag", "value": FLAG.strip()}):
            return '<script>alert("wrong??");history.go(-1);</script>'
        return '<script>alert("good");history.go(-1);</script>'
  

memo_text = ""

  
@app.route("/memo")
def memo():
    global memo_text
    text = request.args.get("memo", "")
    memo_text += text + "\n"
    return render_template("memo.html", memo=memo_text)
  

app.run(host="0.0.0.0", port=8000)
```

이때 각각의 함수는 다음의 역할을 수행한다. 

- `vuln` - `render_template` 함수를 사용해 **XSS가 발생하지 않게 함**
- `memo` - 사용자가 전달한 `memo` 파라미터의 값을 `render_template` 함수를 통해 기록
- `flag` - 메소드에 따라 요청마다 다른 기능을 수행
    - `GET` - 사용자에게 URL을 입력받는 페이지를 제공
    - `POST` - `param` 파라미터의 값과 쿠키에 FLAG를 포함해 `check_xss` 함수를 호출하며, `check_xss` 함수는 `read_url` 함수를 호출해 `vuln` 엔드포인트에 접속

이 때 `vuln` 함수가 제공하는 vuln 페이지에 들어가 보면 알겠지만, param에 `<script>alert(1)</script>`이 있음에도 `alert(1)`이 동작하지 않아 XSS가 발생하지 않는 것을 확인할 수 있다. 

즉, 이 문제에서는 전체적으로 `<script>` 태그를 통한 XSS가 먹히지 않는다는 것을 알 수 있다. 이 때`check_xss` 함수에서 `read_url` 함수를 호출해 `vuln` 엔드포인트에 접속하기 때문에, XSS를 사용하기 위해서는 다른 방법을 사용해야 할 필요가 있다. 

**(`read_url` 함수를 통해 vuln 페이지에 접근하는 URL과 사용자의 쿠키가 전달된다.)**

---
### 취약점 분석
이전과는 다르게, vuln 페이지에 존재하는 innerHTML을 통해 `<script>` 태그로 XSS를 발생시킬 수 없는 상황이라는 것을 알았다. 이와 관련해서 [innerHTML 관련 문서](https://developer.mozilla.org/en-US/docs/Web/API/Element/innerHTML#security_considerations)를 확인해 보는 것을 추천한다. 따라서 다른 XSS 공격 벡터를 시도해봐야 한다. 

이때 innerHTML은 웹 개발에서 사용하는 **DOM(Document Object Model)** 의 속성 중 하나로, HTML 문서의 구조와 내용을 프로그래밍적으로 조작하고자 할 때 JavaScript를 통해 DOM을 사용한다. 

| 코드                                                              | 설명                   |
| --------------------------------------------------------------- | -------------------- |
| `var content = document.getElementById('example').innerHTML;`   | 읽기: 요소의 내부 HTML을 가져옴 |
| `document.getElementById('example').innerHTML = 'New Content';` | 쓰기: 요소의 내부 HTML을 변경함 |
innerHTML을 사용하면 요소의 내부 HTML을 문자열로 읽거나 설정할 수 있지만, 이렇게 사용자의 입력을 바탕으로 innerHTML을 설정했을 때 XSS 공격에 취약할 수 있다. 

이 때 동일한 취약점이 문제 파일로 받은 *vuln.html*에도 존재하는데, 다음 부분이다. 

```html
{% extends "base.html" %}
{% block title %}Index{% endblock %}

{% block head %}
  {{ super() }}
  <style type="text/css">
    .important { color: #336699; }
  </style>
{% endblock %}
  
{% block content %}
    <div id='vuln'></div>
    <script>var x=new URLSearchParams(location.search); document.getElementById('vuln').innerHTML = x.get('param');</script>
{% endblock %}
```

이 때, `<script>` 태그 부분에서 현재 페이지의 URL 쿼리 문자열에서 `param` 파라미터의 값을 추출하고, 해당 값을 페이지 내의 `vuln` 이라는 ID를 가진 요소의 innerHTML로 설정하는 코드를 `{% block content %}` 부분에서 확인할 수 있다. 

이는 innerHTML을 통해 사용자가 URL의 `param` 쿼리 파라미터를 조작해서 웹 페이지의 내용을 변경할 수 있게 한다는 점에서 XSS에 취약함을 알 수 있다. 

---
## 익스플로잇
문제를 해결하기 위해서는 *vuln.html*에서 발생하는 XSS 취약점을 통해 임의 사용자의 쿠키를 탈취해야 한다. 

이때 `<img src>` 태그를 사용해 다음과 같은 익스플로잇 코드를 작성할 수 있다.

```html
<img src='x' onerror="location.href='/memo?memo=' + document.cookie">
```

이 익스플로잇 코드를 `/flag` 페이지의 `param` 부분에 입력하면 `/memo` 페이지에서 임의 사용자의 쿠키 정보를 확인해 플래그를 획득할 수 있다. 

또한, `/vuln` 페이지의 `param`에 해당 익스플로잇 코드를 입력하면 이미지 로딩에 실패한 화면이 다음과 같이 보여지는 것을 확인할 수 있는데, 이는 가져올 이미지의 주소를 적는 `<img src=` 부분에 `'x'`라는 이미지 주소가 아닌 값을 적어 의도적으로 에러를 발생시켰기 때문이다. 

- `<img src=` : 나타낼 이미지의 주소
- `onerror=` : 이미지를 가져오는 것에 에러가 발생했을 경우 실행할 코드
- `location.href`: 해당 페이지로 이동
- `document.cookie`: 사용자의 쿠키 값

![[Pasted image 20260127152728.png]]

---
## 마치며
이번 강의에서는 xss-2 문제를 통해 쿼리 파라미터를 조작해서 웹 페이지의 내용을 변경할 수 있게 하는 취약점이 발생할 수 있는 문제에 대해 알아보았다. 

xss-2 문제와 같이 사용자의 입력을 바탕으로 innerHTML을 설정했을 때 XSS 공격에 취약할 수 있으므로, 사용자 입력을 받기 전에 서버 측에서 사용자의 입력값을 검증하거나 이스케이프 처리를 하는 방식 등을 사용하는 게 추천된다. 