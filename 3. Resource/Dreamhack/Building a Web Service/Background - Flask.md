## 들어가며
---
웹 서버는 사용자가 요청한 웹 페이지를 찾아 사용자에게 보여주는 중요한 역할을 한다. 다시 말하면, 웹 서버를 만들지 못한다면 아무리 멋지고 좋은 웹 페이지를 만들어도, 인터넷을 통해 다른 사람에게 보여 줄 방법이 없게 된다. 

이 강의에서는 웹 서버의 개념, **Flask**라는 웹 프레임워크를 사용해 실제 작동하는 웹 서버를 만드는 실습 등을 공부한다. 

>[!example] 왜 Flask인가?
>![[Pasted image 20260106171540.png]]
>
>웹 서버를 구축하는 방법은 다양하지만, Flask는 특히 Python이라는 친숙한 프로그래밍 언어를 바탕으로 하기 때문에 비교적 배우기 쉽고 직관적인 구조를 가지고 있다. 또한, Flask는 가볍고 유연한 특성 덕분에 다양한 프로젝트에 적합하다. 
>
>본 강의에서는 Flask로 다음을 배운다. 
>
>- 브라우저와 서버 간의 소통 원리 이해하기
>- HTTP request와 response의 흐름 파악하기
>- 웹 서버가 어떻게 동작하는지, 그리고 Flask를 사용해 이를 어떻게 직접 구현하는지


## 웹 서버
---
### 웹 서버란?
---
**웹 서버(Web Server)** 란, 요청을 받으면 응답을 돌려주는 프로그램으로, 웹 브라우저를 사용해서 웹 서핑을 하는 상황에서 어떤 웹 사이트의 각각 다른 메뉴에 접근하면 다른 웹 페이지가 보인다. 

![[Pasted image 20260106172505.png]]

이는 논리 흐름 상 웹 서버가 요청된 경로에 따라서 각각 다른 웹 페이지를 반환하기 때문이다. 


### 웹 서버는 어떻게 만들어질까?
---
일반적으로 웹 서버는 **웹 프레임워크(Web Framework)** 라고 하는, <u>웹 개발을 위해 반복적이거나 기본적으로 필요한 기능과 뼈대를 제공하는 개발 환경</u>을 사용해서 이루어진다. 

만약, 우리가 아무런 프레임워크의 도움 없이 웹 개발을 해야 한다면, 다음 과정들을 전부 구현해야 한다. 

1. 브라우저와 TCP 소켓 통신을 수행한다. 
2. 소켓으로부터 데이터를 받는다. 
3. 받은 데이터가 HTTP 프로토콜 표준에 맞는 HTTP request인지 검사한다.
4. HTTP request에서 어떤 경로의 데이터를 원하는지 탐색한다.
5. 추출된 경로에 맞는 HTML 문서를 파일 입출력으로 연다.
6. 해당 HTML 문서를 읽어 메모리에 저장한다.
7. HTTP 프로토콜 표준에 맞게 HTTP response를 작성한다.
8. 메모리에 저장된 문서를 HTTP response에 넣는다.
9. 구성된 HTTP response를 앞서 맺어진 소켓을 통해 브라우저에게 전송한다. 

물론, [[보안네트워크 프로그래밍]] 수업(3학년 2학기 수업)에서 공부했듯, C나 Python으로 해당 작업들을 전부 구현하는 게 불가능한 것은 아니지만, 이 모든 것을 전부 구현하려면 ***네트워크 프로그래밍, 시스템 프로그래밍, 데이터 비교, 특정 위치의 데이터 추출, 파일 입출력 등*** 다양한 기술이 필요하고, 구현 과정에서 실수가 발생하면 안 되기 때문에 숙련도도 필요하다. 

![[Pasted image 20260106183719.png]]

물론, 프레임워크를 알아도, 해당 지식들을 아예 몰라도 된다는 것은 아니다. 


### 웹 프레임워크
---
웹 프레임워크는 앞서 언급한 9단계의 과정을 기본적으로 대신해서 처리해 준다. 프로그래밍 분야에는 "Do not reinvent the wheel"이라는 격언이 있는데, "바퀴의 재발명"이란 뜻으로, 이미 있는 것을 냅두고 굳이 처음부터 모든 걸 구현하는 것은 비효율적인 일이라는 의미이다. 

웹 프레임워크도 이러한 관점에서 사용하는 것으로, 소켓 통신, HTTP 프로토콜 검사 등의 과정은 이미 반복화할 수 있고 표준화된 작업이라 다시 개발할 필요가 없다. 

따라서 웹 프레임워크를 사용하면 개발자는 불필요한 과정을 신경쓰지 않고 오직 웹 서버의 논리 흐름 설계에 집중할 수 있어 생산적인 개발이 가능하다. 

예를 들어, 간단한 웹 페이지 하나를 제공하는 프로그래밍을 할 때, Flask 프레임워크의 도움을 받으면 아래의 10줄짜리 Python 코드로도 구현이 가능하다.

```python
from flask import Flask, render_template   # Flask 웹 프레임워크 모듈을 불러옵니다.

app = Flask(__name__)

@app.route('/home')                        # /home 경로로의 요청이 들어오면,
def get_home():                            # get_home()을 실행합니다.
    return render_template('home.html')    # HTML 문서인 home.html을 반환합니다.

if __name__ == '__main__':
    app.run(host='0.0.0.0')                # 웹 서버를 구동합니다.
```


## Flask
---
### Flask
---
이번 강의에서 배울 Flask는 **Python**을 기반으로 하는 웹 프레임워크로, Python의 모듈 형태로 제공되며 Python 특유의 심플한 문법과 편리한 사용성을 그대로 활용할 수 있다. 

![[Pasted image 20260106185556.png]]

특히 Flask는 가볍고 유연한 구조 덕분에 배우기 쉽고, 빠르게 웹 애플리케이션을 개발할 수 있다. 보통 컴퓨터 과학이나 보안을 공부하는 사람들이 흔히 첫 번째 또는 두 번째 프로그래밍 언어로 Python을 배우기 때문에 Python의 친숙함으로 학습 난이도가 상대적으로 낮은 편이다. 

또한, Flask는 워게임이나 CTF에서 자주 활용되는 웹 프레임워크 중 하나이다. Flask를 기반으로 제작되는 문제들이 있기 때문에, Flask를 알면 문제를 풀 때도 수월하다. 

이번 강의에서는 Flask를 사용해 웹 서버를 개발하는 방법을 배우는 것이 목표이며, 이를 위해 Flask를 사용하는 방법을 익혀보고, 예시 웹 서버를 실제로 구축하는 실습까지 진행한다. 


### 실습: Flask 설치하기
---
이번 강의는 x86-64 아키텍처에서 Ubuntu 22.04 64-bit 환경을 기준으로 하며, Python3와 `pip` 명령어가 설치되어 있다고 가정한다. 강의를 원활하게 수강하려면, 동일한 환경을 추천한다. 

**Flask 설치 명령어 실행**
Windows 환경에서는 Win + R로 cmd 창을 켜서 `pip install flask` 명령어를 실행하며, Linux 환경에서는 터미널을 켜서 `$ python3 -m pip install flask` 명령어를 실행한다. 

**Flask 설치 확인**
이후, `pip` 명령어에 `pip list | grep "Flask"` 명령어를 입력하면, Flask 모듈이 성공적으로 설치되어 있는 경우 Flask 모듈의 버전이 터미널에 출력된다.


### 실습: Flask 사용해보기
---
이번 실습에서 사용할 파일은 다음과 같다. 

- *app.py*
- *templetes/simple_page.html*

***app.py***
사용자가 브라우저를 통해 `/simple_page` 경로로 request를 보내면 웹 페이지를 반환한다.

```python
from flask import Flask, render_template          # Flask 웹 프레임워크 모듈을 불러옵니다.

app = Flask(__name__)

@app.route('/simple_page')                        # /simple_page 경로로의 요청이 들어오면,
def get_simple_page():                            # get_simple_page()를 실행합니다.
    return render_template('simple_page.html')    # HTML 문서인 simple_page.html을 반환합니다.

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=31337)           # 웹 서버를 구동합니다.
```

***templetes/simple_page.html***
```html
<!DOCTYPE html>
<html>
	<head>
		<title>My Simple Web Page></title>
	</head>
	<body>
		<h1>Explore the World of HTML!</h1>
		<p>Greetings from the Web!</p>
	</body>
</html>
```


### 실습: Flask 사용해보기 - Flask 디렉터리 구성하기
---
Flask 웹 서버가 정상적으로 동작하려면, *app.py* 와 *templates/simple_page.html* 파일들이 올바른 디렉터리에 위치해 있어야 한다. 

*app.py*와 *templates/simple_page.html* 파일은 아래의 디렉터리 구조로 배치되어야 한다. 

```
flask-app                  / 아무 이름이든지 상관 없음
├── app.py                 / 웹 서버 파일
└── templates              / HTML 문서들을 담는 디렉터리로, 별도로 설정하지 않는 이상 반드시 templates라는 이름으로 존재
    └── simple_page.html   / HTML 문서 파일
```


### 실습: Flask 사용해보기 - Flask 실행하기
---
이제 Flask 서버를 구동해 보자. 

1. 터미널을 켜서 해당 파일들이 있는 디렉터리로 이동한다 (여기서는 `flask-app`).
2. *app.py*를 Python으로 실행하면 Flask 웹 서버가 구동된다. 

터미널로 실행 중일 때는, 터미널을 끄면 웹 서버도 종료되므로 터미널이 꺼지지 않아야 한다. 

![[Pasted image 20260106192452.png]]
위의 이미지에서 터미널에 출력되는 텍스트를 살펴보자. 

- `Running on all addresses (0.0.0.0)`
	- Flask 서버가 모든 IP 주소로부터 request를 받을 수 있음
		- `0.0.0.0`: 특정 IP에 한정하지 않음
	- 이 문구가 뜨면 다른 외부 네트워크의 사용자가 해당 서버로 접속할 수 있으며, 반대로 이 문구가 뜨지 않으면 외부 네트워크의 사용자가 해당 서버로 접속할 수 없음
- `Running on http://127.0.0.1:31337`
	- 로컬호스트(Localhost, 컴퓨터 자기 자신)에서도 웹 서버에 접근할 수 있음
	- 웹 서비스가 31337 포트로 열려 있으며, URL은 `http://127.0.0.1:31337`임
- `Running on http://172.16.0.3:31337`
	- 웹 서버의 특정 네트워크 인터페이스에 할당된 로컬 IP 주소
	- 로컬 네트워크 내에서도 해당 서버에 접근할 수 있음


### 실습: Flask 사용해보기 - 브라우저로 웹 서버 테스트해보기
---
위에서 웹 서버를 열었으므로, 브라우저를 사용해 정말로 접속이 가능한지 테스트 해 보자. 

1. 웹 서버를 켜둔 터미널은 그대로 두고, 그 상태로 브라우저를 켠다. 
2. 브라우저의 URL 바에 `http://127.0.0.1:31337/simple_page` 주소를 입력한다.

웹 서버에 성공적으로 요청이 보내지면 브라우저에는 웹 페이지가 정상적으로 표시되어야 하고, 서버를 켜 놓은 터미널에는 `/simple_page` 경로로 요청을 전송했다는 로그가 출력되어야 한다.

![[Pasted image 20260106211444.png]]


### 실습: Flask 사용해보기 - app.py 코드 분석해보기
---
이번 실습에서 사용한 코드는 가장 기본적인 형태의 Flask 코드로, 강의가 진행되면서 본 코드에서 점점 뭔가가 추가되는 형태로 확장할 것이기 때문에 반드시 이해하고 넘어가야 한다. 

```python
from flask import Flask, render_template          # Flask 웹 프레임워크 모듈을 불러옵니다.

app = Flask(__name__)

@app.route('/simple_page')                        # /simple_page 경로로의 요청이 들어오면,
def get_simple_page():                            # get_simple_page()를 실행합니다.
    return render_template('simple_page.html')    # HTML 문서인 simple_page.html을 반환합니다.

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=31337)           # 웹 서버를 구동합니다.
```

**1번 줄**
Python에서 `flask` 모듈에서 `Flask` 클래스와 `render_template` 함수를 가져온다. 

**3번 줄**
`Flask` 클래스의 인스턴스 = 객체를 생성해 `app` 변수에 할당한다. 

**5번 줄**
Python의 데코레이터(Decorator) 기능을 활용한 코드인데, `/simple_page` 경로로 들어오는 HTTP request를 바로 아래 함수인 `get_simple_page()`로 연결한다.

**6~7번 줄**
데코레이터에 의해 HTTP request가 들어오면, *simple_page.html*이라는 이름의 HTML 파일을 `render_template()`로 렌더링 해서 사용자에게 반환한다.

**9~10번 줄**
메인 함수로, Flask 웹 서버를 실행한다. 또한 `host` 인자를 `0.0.0.0` 으로 설정하여 외부의 모든 사용자가 해당 서버에 접근할 수 있도록 하며, `port` 인자를 `31337`로 설정해 31337번 포트에서 웹 서버가 구동할 수 있도록 한다.


## 템플릿 렌더링
---
### 템플릿 렌더링
---
Flask에서는 `render_template()` 함수를 사용해 **템플릿 렌더링(Template Rendering)** 을 할 수 있다. 템플릿 렌더링이란, <u>서버의 Python 코드에서 생성한 변수나 값을 뼈대인 HTML 코드에 삽입해 최종 HTML 문서를 만드는 것</u>을 말한다. 

- **HTML 템플릿**: 뼈대인 HTML 코드로, HTML 파일이지만, 실제 데이터가 채워지기 이전의 뼈대
- **최종 HTML 문서**: HTML 템플릿에 실제 데이터가 채워져 완성된 HTML 파일

강의 전반적으로 두 단어가 반복적으로 등장한다.


### 템플릿 렌더링 예시 살펴보기
---
예시를 통해 템플릿 렌더링에 대해서 살펴보자.

- *app.py*
- *templates/simple_page.html*

아래 코드들은 앞의 실습에서 다뤘던 코드를 조금 수정해서 템플릿 렌더링을 적용한 버전으로, 직접 따라하지 않고 읽고 넘어가도 상관없다. 

***app.py***
```python
import datetime
from flask import Flask, render_template

app = Flask(__name__)

@app.route('/simple_page')
def get_simple_page():
    dt = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    return render_template('simple_page.html', current_time=dt)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=31337)
```

`render_template()` 함수의 두 번째 매개변수인 `current_time`에 `dt`라는 변수가 전달되도록 하였으며, 이는 `dt`라는 현재 시각을 문자열 형태로 담고 있는 변수를 `current_time`이라는 이름으로 *simple_page.html*에 전달한다는 의미이다.

***templates/simple_page.html***
```html
<!DOCTYPE html>
<html>
	<head>
	  <title>My Simple Web Page</title>
	</head>
	<body>
	  <h1>Explore the World of HTML!</h1>
	  <p>Greetings from the Web!</p>
	  <h2>{{ current_time }}</h2>
	</body>
</html>
```

`{{ current_time }}`이라는 문구를 `<h2>` 태그로 출력하는 요소가 추가되었다.

**브라우저 테스트 결과**
![[Pasted image 20260106220454.png]]
위의 *app.py, templates/simple_page.html*로 웹 서버를 연 후 브라우저로 `http:/127.0.0.1:31337/simple_page`에 접속하면 위의 사진과 같은 웹 페이지가 나온다.

이는 앞서 살펴봤듯이 *app.py*에서 `render_template()`를 통해 현재 시각을 나타내는 문자열을 `current_time`으로 *simple_page.html*에 전달했고, *simple_page.html*은 전달받은 `current_time`을 이중 중괄호로 감싼 `{{ current_time }}`이란 특수한 문구로 삽입했기 때문이다. 


### 템플릿 렌더링 기호
---
`render_template()`는 HTML 템플릿에 존재하는 특수한 기호들을 규칙에 맞게 렌더링한다. 예를 들어, 앞서 살펴본 예시에서는 변수의 이름을 이중 중괄호로 감싼 `{{ 변수이름 }}` 형태를 HTML 템플릿에 작성하면 `변수이름`에 해당하는 데이터를 최종 HTML 문서에 삽입할 수 있었다. 

이는 `render_template()`가 내부적으로 [Jinja2](https://jinja.palletsprojects.com/en/stable/)라는 템플릿 엔진을 사용하는데, 이 엔진의 규칙에 따라 기호를 렌더링하기 때문이다. 


### 템플릿 렌더링 기호 - {{ 표현식 }}
---
HTML 문서에 `{{ 표현식 }}` 형태로 작성된 [표현식(Expression)](https://tedboy.github.io/jinja2/templ13.html)인 `표현식`을 실행한 결과를 삽입할 수 있다. 예시로는 다음과 같다. 

- *app.py*
- *templates/expression.html*

***app.py***
사용자가 `/expression` 경로에 접근하면 `get_expression()`을 실행한 결과를 반환한다. `get_expression()`에서는 `render_template()`를 실행하며, 11번 줄에서 볼 수 있듯이 총 4개의 변수 `x`, `y`, `z`, `arr`을 각각 동일한 이름으로 템플릿인 *expression.html*에 전달한다.

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route('/expression')
def get_expression():
    x = 1
    y = 10
    z = 100
    arr = [2, 4, 8, 16]
    return render_template('expression.html', x=x, y=y, z=z, arr=arr)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=31337)
```

***templates/expression.html***
`{{ 표현식 }}`의 다양한 예시를 보여 준다.

```html
<!DOCTYPE html>
<html>
	<head>
		<title>Expression Usages</title>
	</head>
	<body>
		<h1>Expression Usages</h1>
		<p>x: {{ x }}</p>
		<p>x + 123: {{ x + 123 }}</p>
		<p>123 + 123: {{ 123 + 123 }}</p>
		<p>y + z: {{ y + z }}</p>
		<p>arr[1]: {{ arr[1] }}</p>
	</body>
</html>
```

웹 서버를 구동한 후 브라우저를 통해 `http://127.0.0.1:31337/expression` 페이지에 접근하면 아래 화면과 같은 웹 페이지가 나온다. 

![[Pasted image 20260106232430.png]]


### 템플릿 렌더링 기호 - {% 구문 %}
---
`render_template()`는 표현식 뿐만 아니라 다양한 구문도 지원한다. 이번에는 C와 Python에서 접할 수 있는 조건문과 반복문에 더해 **다른 템플릿을 상속**해서 사용하는 구문까지 공부한다. 


### 템플릿 렌더링 기호 - {% 구문 %} - 조건문
---
**조건문**은 주어진 조건의 참 또는 거짓에 따라 HTML 템플릿 문서에 데이터를 다르게 삽입할 수 있는 구문으로, C나 Python에서 접할 수 있는 `if`, `if-else`, `if-else if-else`문 모두 사용할 수 있다. 또한, 조건문은 `{% endif %}`로 끝나야 한다. 

예시는 아래와 같다. 

```html
{% if 조건1 %}
    <!-- 조건1이 참일 때 실행 -->
{% elif 조건2 %}
    <!-- 조건2가 참일 때 실행 -->
{% else %}
    <!-- 모든 조건이 거짓일 때 실행 -->
{% endif %}
```

실제 코드로 예시를 살펴보자. 일일히 쓰기 귀찮으니, 각각 *app.py*와 *templates/example.html*이다. (해당 구조는 앞으로도 쭉 적용된다)

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route('/example')
def get_stmt_condition():
	user_role = 'member' # 예시: 'admin', 'member', 'guest'
	return render_template('example.html', role=user_role)
	
	
if __name__ == '__main__':
	app.run(host='0.0.0.0', port=31337)
```

```html
<!DOCTYPE html>
<html>
	<head>
		<title>Condition Statement Usages</title>
	</head>
	<body>
		<h1>Condition Statement Usages</h1>
		<h2>User role</h2>
		{% if role == 'admin' %}
			<p>You are an administrator.</p>
		{% elif role == 'member' %}
			<p>You are a regular member.</p>
		{% else %}
			<p>You are using a guest account.</p>
		{% endif %}
	</body>
</html>
```

두 파일로 인해 구동한 웹 서버를 브라우저를 통해 실행하면 다음과 같은 화면이 나온다.

![[Pasted image 20260107002849.png]]
### 템플릿 렌더링 기호 - {% 구문 %} - 반복문
---
**반복문**을 활용하면 서버 Python 코드에서 전달된 리스트, 딕셔너리 등의 데이터를 순회하면서 HTML 컨텐츠를 동적으로 생성할 수 있다. Jinja2의 반복문은 다음 예시처럼 Python과 비슷하다. 

```html
<ul>
	{% for 아이템명 in 순회 가능 데이터 %}
		<li>{{ 아이템명 }}</li>
	{% endfor %}
</ul>
```

예시는 아래와 같다. 

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route('/example')
def get_stmt_iteration():
	fruits = ['Apple', 'Banana', 'Cherry', 'Melon', 'Lemon']
	return render_template('example.html', l=fruits)
	
	
if __name__ == '__main__':
	app.run(host='0.0.0.0', port=31337)
```

```html
<!DOCTYPE html>
<html>
	<head>
		<title>Iteration Statement Usage</title>
	</head>
	<body>
		<h1>Iteration Statement Usage</h1>
		<h2>Fruit List</h2>
		<ul>
			{% for element in l %}
				<li>{{ element }}</li>
			{% endfor %}
		</ul>
	</body>
</html>
```

두 파일로 인해 구동한 웹 서버를 브라우저를 통해 실행하면 다음과 같은 화면이 나온다.

![[Pasted image 20260107004918.png]]
### 템플릿 렌더링 기호 - {% 구문 %} - 템플릿 상속 구문
---
**템플릿 상속 구문**은 여러 HTML 파일 간에 공통적인 템플릿을 공유하고 재사용할 수 있도록 하는 기능이다. 상속 기능을 사용하면 코드 중복을 줄이고, 유지보수도 편하다는 장점이 있다. 

템플릿 상속 기능을 사용하려면 1) **부모 템플릿**이 있고, 해당 템플릿을 상속받을 2) **자식 템플릿**이 필요하다. 부모 템플릿을 자식 템플릿이 상속받을 때 코드 중복을 줄이고 전체적인 코드 구조를 일관적으로 관리할 수 있어 유지보수가 편해진다. 

- **{% block 블록명 %} {% endblock %}**
	해당 구문은 <u>부모 템플릿과 자식 템플릿 모두</u> 사용할 수 있다. 부모 템플릿에서는 자식 템플릿에서 덮어쓸 부분을 정의하며, 자식 템플릿에서는 부모 템플릿에 반영할 데이터를 지정할 수 있다.

- **{% extends '부모템플릿파일명' %}**
	해당 구문은 <u>자식 템플릿에서</u> 사용하며, 이 구문을 사용해 부모 템플릿을 상속 받는다.

예시를 통해 알아보자. 이번에도 소스코드는 순서대로 보여 준다.

- *app.py*
- *templates/base.html*
- *templates/home.html
- *templates/about.html*

이번에는 `/home` 경로에 접근할 시 *home.html*을 렌더링하며, `/about` 경로에 접근할 시 *about.html*을 렌더링해 보여주는 웹 서비스를 예시로 보여 준다. 이 때, *base.html*은 나머지 두 개의 부모 템플릿이 된다.

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route('/home')
def get_home():
	return render_template('home.html')
	
	
@app.route('/about')
def get_about():
	return render_template('about.html')
	
	
if __name__ == '__main__':
	app.run(host='0.0.0.0', port=31337)
```

```html
<!DOCTYPE html>
<html>
	<head>
	  <title>{% block title %}Default Title{% endblock %}</title>
	</head>
	<body>
	  <h1>Common Site Heading</h1>
	  
	  {% block content %}
		<p>Default Content</p>
	  {% endblock %}
	  
	  <p>Common Site Footer Text</p>
	</body>
</html>
```

```html
{% extends 'base.html' %}

{% block title %}
  Home Page
{% endblock %}

{% block content %}
  <h2>Welcome to the Home Page!</h2>
  <p>This is the home page.</p>
{% endblock %}
```

```html
{% extends 'base.html' %}

{% block title %}
    About Page
{% endblock %}

{% block content %}
  <h2>Welcome to the About Page!</h2>
  <p>This is the about page.</p>
{% endblock %}
```

*base.html*을 상속받은 자식 HTML 템플릿은 다음 두 군데의 내용을 덮어쓸 수 있다. 

- **{% block title %}Default Title{% endblock %}**
	- `Default Title` 대신에 다른 문구를 넣을 수 있다.
	- 다른 문구를 넣지 않은 경우, `Default Title`이 그대로 출력된다.
- **{% block content %}`<p>`Default Content`</p>`{% endblock %}**
	- `<p>Default Content</p>` 대신 다른 내용을 넣을 수 있다. 
	- 다른 내용을 넣지 않은 경우, `<p>Default Content</p>`가 그대로 출력된다.

구동한 웹 서버를 브라우저를 통해 실행하면 다음 두 가지의 결과가 화면에 표시된다. 

![[Pasted image 20260107011004.png|350]]![[Pasted image 20260107011017.png|350]]

### 템플릿 렌더링 기호 - {# 주석 #}
---
주석은 Flask 템플릿에서 최종 결과물에 출력되지 않으며, 참고 정보나 메모 등을 작성할 때 많이 사용한다. 예시 HTML 템플릿 문서 하나만 살펴본 후 바로 넘어간다. 

```html
<!DOCTYPE html>
<html>
	<head>
	  <title>Flask 템플릿</title>
	  {# 이곳은 사용자에게 표시되지 않는 주석입니다. #}
	</head>
	<body>
	  <h1>템플릿 주석 예제</h1>
	  {# 이곳도 사용자에게 표시되지 않는 주석입니다. #}
	  <p>이 문장은 사용자에게 표시됩니다.</p>
	</body>
</html>
```

물론, HTML에서도 다음처럼 주석을 제공하기는 한다. 

```html
<!-- 이 문장은 주석 처리된 문장 -->
```

그런데, <u>HTML 주석은 브라우저 웹 페이지 화면에만 안 보일 뿐, 소스 코드에는 그대로 노출이 되는 정보이다.</u> 하지만, Jinja2에서 사용하는 `{# 주석 #}`은 렌더링 과정에서 제거되기 때문에 최종 HTML 소스 코드에 노출되지 않는 장점이 있다. 


## 사용자로부터 입력받기
---
### 들어가기에 앞서
---
이번 주제에서는 **사용자로부터 입력을 받는 세 가지 주요 방식**을 살펴본다. 

지금까지 우리가 만든 웹 서버는 사용자가 요청한 HTML 파일을 그대로 보여주는 역할만 하고 있다. 예를 들어, `/about` 경로로 접근하면 회사 소개 페이지가, `/contact`로 접근하면 문의하기 페이지가 보이는 정도의 기능만 하고 있다는 의미이다. 

하지만 실제 대부분의 웹 서버는 사용자와 상호작용을 할 수 있어야 한다. 로그인 기능을 예시로 들면, 사용자가 ID와 PW를 가지고 로그인을 시도하면 웹 서버가 이를 확인하고 맞는 경우 "로그인 성공" 메시지를, 틀린 경우 "로그인 실패" 메시지를 보여줘야 한다. 게시판 기능도 마찬가지다.
사용자가 글을 작성하거나 댓글을 달 수 있어야 하고, 이를 서버가 저장하고 있다가 사용자에게 보여줘야 한다. 


### 실습: URL 경로 매개변수를 통한 입력 처리
---
이번 실습에서는 **URL 경로 매개변수(URL Path Parameter)** 를 통해 사용자로부터 입력을 어떻게 받는지 배운다. 

>[!example] URL 경로 매개변수(URL Path Parameter)란?
>데이터를 URL 경로의 일부로 포함시켜 서버에 전달하는 방식이다. 
>
>예를 들어 다음의 URL이 있다고 가정하자. 
>```
>https://example.com/products/123
>```
>
>이 때, `/products`는 상품 목록을 나타내는 경로, `123`은 특정 상품의 ID를 나타내는 <u>매개변수</u>의 인자값이다. 이 때 `123`이 위치한 곳이 경로 매개변수이며, `123` 대신에 `456`을 넣으면 상품의 ID가 `456`인 특정 상품을 조회할 수 있다.

이번 장에서도 순서대로 *app.py*와 *templates/result.html*을 설명을 위해 사용한다. 

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route('/welcome/<name>')
def get_welcome_name(name):
	return render_template('result.html', name=name)
	
	
if __name__ == '__main__':
	app.run(host='0.0.0.0', port=31337)
```

```html
<!DOCTYPE html>
<html>
	<head>
		<title>URL Path Parameter Example</title>
	</head>
	<body>
		<h1>URL Path Parameter Example</h1>
		<h2>Hello, {{ name }}!</h2>
	</body>
</html>
```

구동한 웹 서버를 브라우저를 통해 `http://127.0.0.1:31337/welcome/` 뒤에 아무 이름이나 넣고 돌려 보면 다음의 결과들이 나온다. 

![[Pasted image 20260108223035.png|350]]![[Pasted image 20260108223050.png|350]]

### 실습: URL 쿼리 매개변수를 통한 입력 처리
---
이번 실습에서는 **URL 경로 매개변수(URL Path Parameter)** 를 통해 사용자로부터 입력을 어떻게 받는지 배운다. 

>[!example] URL 쿼리 매개변수(URL Query Parameter)란?
>URL의 `?` 뒤에 오는 매개변수로, 사용자가 데이터를 서버로 전달하는 방식 중 하나이다.
>
>예를 들어 다음의 URL이 있다고 가정하자. 
>```
>http://example.com/search?query=flask&page=2
>```
>여기서 `?` 뒤에 오는 `query`가 **매개변수 키(Key)** 를 나타내며, `flask`가 **매개변수 값(Value)** 이다. `&`으로 이어진 `page=2`는 또 다른 매개변수 키-값 쌍을 의미한다.

이번 장에서도 순서대로 *app.py*와 *templates/result.html*을 설명을 위해 사용한다. 

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route('/welcome')
def get_welcome():
	# request.args: 사용자로부터 전달받은 쿼리 매개변수를 담고 있는 객체
	# 해당 객체의 get 메서드를 사용해 user_name이라는 매개변수를 가져와
	# user_name에 대입
	user_name = request.args.get('user_name')
	return render_template('result.html', name=user_name)
	
	
if __name__ == '__main__':
	app.run(host='0.0.0.0', port=31337)
```

```html
<!DOCTYPE html>
<html>
	<head>
		<title>URL Query Parameter Example</title>
	</head>
	<body>
		<h1>URL Query Parameter Example</h1>
		<h2>Hello, {{ name }}!</h2>
	</body>
</html>
```

구동한 웹 서버를 브라우저를 통해 `http://127.0.0.1:31337/welcome?user_name=` 뒤에 아무 이름이나 넣고 돌려 보면 다음의 결과들이 나온다. 

![[Pasted image 20260108233102.png|350]]![[Pasted image 20260108233112.png|350]]

### `<form>` 태그를 통한 POST 데이터 입력 처리
---
HTML의 `<form>` 태그는 사용자가 브라우저 상에서 데이터를 입력한 후 서버로 전송할 수 있는 양식을 웹 페이지에 만든다. 사용자가 `<form>` 태그로 만들어진 양식을 통해 값을 입력해 서버에 전달하면, 서버는 해당 데이터를 알아서 처리해 필요한 작업을 수행할 수 있다. 

해당 기능을 사용하면, 로그인 기능 또한 구현할 수 있다. 


### `<form>` 태그를 통한 POST 데이터 입력 처리 - `<form>` 태그
---
`<form>` 태그에 대해 먼저 알아본 뒤, 사용자의 입력 값이 어떤 식으로 서버에 전달되는지, 그리고 서버는 전달받은 데이터를 어떻게 Python 코드로 처리하는지 알아보자. 

`<form>` 태그는 웹 페이지에 사용자가 값을 입력할 수 있는 양식을 만든다. 이때, 데이터 전송을 위해서 필요한 속성들이 있다. 

| 속성       | 설명                                                                                                  |
| -------- | --------------------------------------------------------------------------------------------------- |
| `action` | 입력 값을 전송할 URL을 지정하는 속성                                                                              |
| `method` | 입력 값을 전송할 때 사용하는 HTTP 메시지의 메서드를 설정한다. GET 메서드와 POST 메서드가 올 수 있는데, <u>이번 장에서는 POST 메서드만 사용</u>할 것이다. |

### `<form>` 태그를 통한 POST 데이터 입력 처리 - 서버에서 처리하기
---

## 마치며
---
