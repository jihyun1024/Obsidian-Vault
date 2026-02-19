## 들어가며
한 번쯤은 캘린더에 일정, 메모 등을 적고 학교에서 들었던 강의를 Obsidian 등의 프로그램으로 정리해 본 경험이 있을 것이다. 새로운 일정이 생기면 캘린더를 열어 날짜에 맞게 기록하고, 일정이 변경되거나 취소되면 기록한 일정을 수정하거나 삭제할 수 있다. 

![[Pasted image 20260204232830.png|350]]

일상생활에서 생기는 다양한 정보를 관리하기 위해 캘린더와 노트, 기타 프로그램을 사용하듯이, 컴퓨터도 정보를 기록하기 위해 **데이터베이스**를 사용하며 데이터베이스를 관리하는 프로그램을 **DataBase Management System (DBMS)** 라고 한다. 

---
## Database Management System
웹 서비스는 데이터베이스에 정보를 저장하고, 이를 관리하기 위해 Database Management System을 사용한다. DBMS는 데이터베이스에 새로운 정보를 기록하거나, 기록된 내용을 수정 또는 삭제하는 역할을 한다. DBMS는 다수의 사람이 동시에 데이터베이스에 접근할 수 있고, 웹 서비스의 검색 기능처럼 복잡한 요구사항을 만족하는 데이터를 조회할 수 있다. 

DBMS에 대해서는 [[Background - 데이터베이스]]에서 DBMS 부분과 RDBMS 부분을 참고하자. 

---
## Relational DBMS
**Relational DataBase Management System (RDBMS)** 는 관계형 데이터베이스로, 1970년에 Codds가 [12가지 규칙](https://en.wikipedia.org/wiki/Codd%27s_12_rules)을 정의해 생성된 데이터베이스 모델이다. 

RDBMS는 행과 열의 집합으로 구성된 테이블의 묶음 형식으로 데이터를 관리하고, 테이블 형식의 데이터를 조작할 수 있는 관계 연산자를 제공한다. Codds는 12가지 규칙을 정의했지만 실제 구현된 RDBMS들은 12가지 규칙을 모두 따르지는 않고, 최소한의 조건으로 앞의 두 조건(*테이블의 묶음 형식으로 관리, 관계 연산자 제공*)을 만족한다. 

RDBMS에서 관계 연산자는 **Structured Query Language (SQL)** 라는 쿼리 언어를 사용하고, 쿼리를 통해 테이블 형식의 데이터를 조작한다. 

---
### SQL
**Structured Query Language (SQL)** 는 RDBMS의 데이터를 정의하고 질의, 수정 등을 하기 위해 고안된 언어이다. SQL은 구조화된 형태를 가지는 언어로 웹 애플리케이션이 DBMS와 상호작용할 때 사용된다. 

SQL은 사용 목적, 행위에 따라 다양한 구조가 존재하며 대표적으로 아래와 같이 구분한다. 

| 언어                                   | 설명                                                                                                   |
| ------------------------------------ | ---------------------------------------------------------------------------------------------------- |
| **DDL (Data Definition Language)**   | <u>데이터를 정의</u>하기 위한 언어이다. 데이터를 저장하기 위한 스키마, 데이터베이스의 CRUD 작업을 수행한다.                                   |
| **DML (Data Manipulation Language)** | <u>데이터를 조작</u>하기 위한 언어이다. 실제 데이터베이스 내에 존재하는 데이터에 대한 CRUD 작업을 수행한다.                                   |
| **DCL (Data Control Language)**      | 데이터베이스의 <u>접근 권한 설정 등</u>을 하기 위한 언어이다. 데이터베이스 내에 사용자의 권한을 부여하기 위한 `GRANT`와 권한을 박탈하는 `REVOKE`가 대표적이다. |

---
### DDL
웹 애플리케이션은 SQL을 사용해서 DBMS와 상호작용하며 데이터를 관리한다. RDBMS에서 사용하는 기본적인 구조는 데이터베이스 ➡️ 테이블 ➡️ 데이터 구조이다. 데이터를 다루기 위해 데이터베이스와 테이블을 먼저 생성해야 하며, DDL을 이때 사용한다. 

DDL의 `CREATE` 명령을 사용해 새로운 데이터베이스 또는 테이블을 생성할 수 있다. 

#### 데이터베이스 생성
Dreamhack이라는 데이터베이스를 생성하는 쿼리문이다. 

```sql
CREATE DATABASE Dreamhack
```

#### 테이블 생성
앞서 생성한 Dreamhack이라는 데이터베이스에 Board 테이블을 생성하는 쿼리문이다. 

```sql
USE Dreamhack;

CREATE TABLE Board(
	idx INT AUTO_INCREMENT,
	boardTitle VARCHAR(100) NOT NULL,
	boardContent VARCHAR(2000) NOT NULL,
	PRIMARY KEY(idx)
);
```

---
### DML
생성된 테이블에 데이터를 추가하기 위해 DML을 사용한다. 

#### 테이블 데이터 생성
Board 테이블에 데이터를 삽입하는 쿼리문이다. 

```sql
INSERT INTO
	Board(boardTitle, boardContent, createdDate)
Values(
	'Hello',
	'World!',
	Now()
);
```

#### 테이블 데이터 조회
Board 테이블의 데이터를 조회하는 쿼리문이다. 

```sql
SELECT 
  boardTitle, boardContent
FROM
  Board
Where
  idx=1;
```

#### 테이블 데이터 변경
Board 테이블의 컬럼 값을 변경하는 쿼리문이다. 

```sql
UPDATE Board SET boardContent='DreamHack!' 
  Where idx=1;
```


---
## 마치며
이번 강의에서는 데이터베이스와 DBMS의 배경 지식에 대해 알아보았다. 만약 데이터베이스 부분의 개념을 잘 모르거나 기억이 안 난다면, [[Background - 데이터베이스]] 노트를 보고 오자. 