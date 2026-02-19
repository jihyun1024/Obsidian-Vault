## 들어가며
MySQL, SQLite 등의 Relational DBMS에 대해서는 앞의 강의인 [[Background - Relational DBMS]] 강의에서 배웠다. 이 RDBMS는 저장해야 하는 데이터가 많아지면 용량이 한계를 넘을 수 있고, 무엇보다 복잡하다. 이를 해결하기 위해 등장한 게 **Non-Relational DBMS (NoSQL, NRDBMS)** 이다. 

Relational DBMS에서 등장할 수 있는 취약점이 SQL Injection이 있었고, NoSQL 또한 사용자의 입력을 통해 동적으로 쿼리를 생성해 데이터를 저장하기 때문에 비슷한 문제점이 발생할 수 있다. 

---
## 비관계형 데이터베이스
RDBMS는 SQL로 데이터를 조회하고 수정할 수 있다. 그러나 NoSQL은 SQL을 사용하지 않고 복잡하지 않은 데이터를 저장해 단순 검색 및 추가 검색 작업을 위해 최적화된 것이 RDBMS와의 차이점이다. 

따라서, RDBMS는 SQL이라는 정해진 하나의 문법만 공부하면 되지만, NoSQL은 Redis, Dynamo, CouchDB, MongoDB 등등 다양한 DBMS가 존재하기 때문에 각각의 구조와 사용 방법을 익혀야 한다는 아주 안 좋은 단점이 존재한다. 

![[Pasted image 20260219221925.png]]

이번 강의에서는 **MongoDB, Redis, CouchDB**에 대해 알아본다. 

---
## MongoDB
MongoDB는 JSON 형태인 Document를 저장하며, 다음의 특징을 가지고 있다. 

- 스키마를 따로 정의하지 않아 각 컬렉션에 대한 정의가 필요 없다.
- JSON 형식으로 쿼리를 작성한다.
- `_id` 필드가 Primary Key의 역할을 한다.

아래 코드는 MongoDB에서 데이터를 삽입하고, 조회하는 쿼리의 예시이다. 

```shell
$ mongosh
> db.user.insertOne({uid: 'admin', upw: 'secretpassword'})
{ acknowledged: true, insertedId: ObjectId("5e71d395b050a2511caa827d")}
> db.user.find({uid: 'admin'})
[{ "_id" : ObjectId("5e71d395b050a2511caa827d"), "uid" : "admin", "upw" : "secretpassword" }]
```

각 DBMS에서 `status`의 값이 "A", `qty`의 값이 30보다 작은 데이터를 찾는 쿼리의 예시이다.

**RDBMS**
```sqlite
SELECT * FROM inventory WHERE status="A" and qty < 30
```

**MongoDB**
```json
db.inventory.find(
	{ $and: [
		{ status: "A" },
		{ qty: { $lt: 30 } }
	]}
)
```

여기서, **컬렉션**이란 데이터베이스의 하위 개념으로, RDBMS에서의 테이블과 비슷한 개념이다. 또한, MongoDB 5.0 버전부터 `mongo` shell은 `mongosh` 명령어로 대체되었다.

![[Pasted image 20260219225744.png]]

---
### MongoDB 연산자
**비교**

| Name   | Description                      |
| ------ | -------------------------------- |
| `$eq`  | 지정된 값과 같은 값을 찾음                  |
| `$in`  | 배열 안의 값들과 일치하는 값을 찾음             |
| `$ne`  | 지정된 값과 같지 않은 값을 찾음 (not equal)   |
| `$nin` | 배열 안의 값들과 일치하지 않는 값을 찾음 (not in) |

**논리**

| Name   | Description                          |
| ------ | ------------------------------------ |
| `$and` | 논리적 AND로, 각각의 쿼리를 모두 만족하는 문서가 반환됨    |
| `$not` | 쿼리 식의 효과를 반전시켜 쿼리 식과 일치하지 않는 문서를 반환함 |
| `$nor` | 논리적 NOR로, 각각의 쿼리를 모두 만족하지 않는 문서가 반환됨 |
| `$or`  | 논리적 OR로, 각각의 쿼리 중 하나 이상 만족하는 문서가 반환됨 |

**지정된 필드**

| Name      | Description             |
| --------- | ----------------------- |
| `$exists` | 지정된 필드가 있는 문서를 탐색함      |
| `$type`   | 지정된 필드가 지정된 유형인 문서를 탐색함 |

**평가식**

| Name     | Description              |
| -------- | ------------------------ |
| `$expr`  | 쿼리 언어 내에서 집계 식을 사용할 수 있음 |
| `$regex` | 지정한 정규식과 일치하는 문서를 선택함    |
| `$text`  | 지정된 텍스트를 검색함             |

---
### 기본 문법
다음은 SQL과 MongoDB의 기본적인 문법을 비교한 내용이다. 

- SELECT
	- SQL
		- `SELECT * FROM account`
		- `SELECT * FROM account WHERE user_id="admin";`
		- `SELECT user_idx FROM account WHERE user_id="admin";`
	- MongoDB
		- `db.account.find()`
		- ```json
		  db.account.find(
			  { user_id: "admin" }
			)
		  ```
		- ```json
		  db.account.find(
			  { user_id: "admin" },
			  { user_idx:1, _id:0 }
			)
		  ```

- INSERT
	- SQL
		- `INSERT INTO account(user_id, user_pw) VALUES ("guest", "guest");`
	- MongoDB
		- ```json
		  db.account.insertOne(
			  { user_id: "guest", user_pw: "guest" }
			)
		  ```

- DELETE
	- SQL
		- `DELETE FROM account`
		- `DELETE FROM account WHERE user_id="guest";`
	- MongoDB
		- `db.account.remove()`
		- ```json
		  db.account.remove(
			  { user_id: "guest" }
			)
		  ```

- UPDATE
	- SQL
		- `UPDATE account SET user_id="guest2" WHERE user_idx=2;`
	- MongoDB
		- ```json
		  db.account.updateOne(
			  { user_idx: 2 },
			  { $set: { user_id: "guest2" } }
			)
		  ```

---
## Redis
Redis는 키-값 쌍을 가진 데이터를 저장한다. 제일 큰 특징은 다른 데이터베이스와 다르게 메모리 기반의 DBMS라는 것으로, 메모리를 사용해 데이터를 저장하고 접근하기 때문에 읽고 쓰는 작업을 다른 DBMS보다 더 빠르게 할 수 있다. 

아래 코드는 Redis에서 데이터를 추가하고 조회하는 명령어의 예시로, Redis에서 데이터를 관리할 때 자주 사용하는 명령어는 표로 명시했으며, [공식 문서](https://redis.io/commands)를 통해 확인할 수 있다. 

~~표는 하나하나 타이핑하기 귀찮아서 그냥 스크린샷 찍었다...~~

**redis 명령어 사용 예시**
```r
$ redis-cli
127.0.0.1:6379> SET test 1234 # SET key value
OK
127.0.0.1:6379> GET test # GET key
"1234"
```

**데이터 조회 및 조작 명령어**
![[Pasted image 20260219232402.png]]

**관리 명령어**
![[Pasted image 20260219232417.png]]


---
## CouchDB
CouchDB 또한 MongoDB처럼 JSON 형태인 Document를 저장하며, 웹 기반의 DBMS로, REST API 형식으로 요청을 처리한다. 다음은 각 메소드에 따른 기능 설명이다. 

| 메소드    | 기능 설명      |
| ------ | ---------- |
| POST   | 새로운 레코드 추가 |
| GET    | 레코드 조회     |
| PUT    | 레코드 업데이트   |
| DELETE | 레코드 삭제     |

아래 코드는 HTTP request로 레코드를 업데이트하고 조회하는 예시이다. 

```shell
$ curl -X PUT http://{username}:{password}@localhost:5984/users/guest -d '{"upw":"guest"}'
{"ok":true,"id":"guest","rev":"1-22a458e50cf189b17d50eeb295231896"}

$ curl http://{username}:{password}@localhost:5984/users/guest
{"_id":"guest","_rev":"1-22a458e50cf189b17d50eeb295231896","upw":"guest"}
```

**특수 구성 요소**
CouchDB에서는 서버 또는 데이터베이스를 위해 다양한 기능을 제공한다. 그 중 `_` 문자로 시작하는 URL, 필드는 특수 구성 요소를 나타낸다. 

다음은 각 구성 요소에 대한 설명으로, 이외의 자세한 정보는 [공식 문서](https://docs.couchdb.org/en/latest/api/index.html)에서 찾아보면 나온다. 

**SERVER**

| 요소          | 설명                  |
| ----------- | ------------------- |
| `/`         | 인스턴스에 대한 메타 데이터를 반환 |
| `/_all_dbs` | 인스턴스의 데이터베이스 목록을 반환 |
| `/_utils`   | 관리자 페이지로 이동         |
**Database**

| 요소                | 설명                                        |
| ----------------- | ----------------------------------------- |
| `/db`             | 지정된 데이터베이스에 대한 정보를 반환                     |
| `/{db}/_all_docs` | 지정된 데이터베이스에 포함된 모든 document 반환            |
| `/{db}/_find`     | 지정된 데이터베이스에서 JSON 쿼리에 해당하는 모든 document 반환 |


---
## 마치며
이번 강의에서는 비관계형 데이터베이스와 NoSQL의 DBMS인 **MongoDB, Redis, CouchDB**에 대해 짤막하게 알아봤다. 다음 시간에는 NoSQL에서 발생하는 서버 사이드 취약점인 NoSQL Injection에 대해서 다룬다. 

![[Pasted image 20260219233500.png]]