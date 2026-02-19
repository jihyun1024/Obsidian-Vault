## 들어가며
[[ServerSide - SQL Injection]] 강의에서 배웠듯이, SQL에서는 **Data Definition Language(DDL), Data Manipulation Language(DML), Data Control Language(DCL)** 이렇게 세 가지의 질의 언어를 제공한다. 

그 중 이번 강의에서는 데이터베이스에서 데이터에 대해 CRUD를 수행하는 구문인 DML에 대해 배운다. 우리가 웹 서비스에서 사용하는 대부분의 기능은 DML을 통해 수행된다 해도 과언이 아닐 정도로 가장 많이 사용되는 구문이다. 

그만큼, DML 구문을 사용하는 목적과 그 형태를 알고 있다면 SQL Injection 공격을 보다 쉽고 빠르게 이해할 수 있다. 

---
## SQL DML
SQL DML에서 CRUD(Create, Read, Update, Delete)를 수행하는 각각의 구문에 대해 알아보자. 

### SELECT
**SELECT**는 데이터를 조회하는 구문으로, 다음은 [MySQL 공식 페이지](https://dev.mysql.com/doc/refman/8.4/en/select.html)에서 제공하는 `SELECT` 구문의 예시이다. 

```sql
SELECT 
	[ALL | DISTINCT | DISTINCTROW ] 
	[HIGH_PRIORITY] 
	[STRAIGHT_JOIN] 
	[SQL_SMALL_RESULT] [SQL_BIG_RESULT] [SQL_BUFFER_RESULT] 
	[SQL_NO_CACHE] [SQL_CALC_FOUND_ROWS]
	 _select_expr_ [, _select_expr_] ... 
	[_into_option_] 
	[FROM _table_references_ 
		[PARTITION _partition_list_]] 
	[WHERE _where_condition_] 
	[GROUP BY [ {_col_name_ | _expr_ | _position_}, ... [WITH ROLLUP] 
				| ROLLUP ({_col_name_ | _expr_ | _position_}, ...)] ] 
	[HAVING _where_condition_] 
	[WINDOW _window_name_ AS (_window_spec_) 
		[, _window_name_ AS (_window_spec_)] ...] 
	[ORDER BY {_col_name_ | _expr_ | _position_} 
		[ASC | DESC], ... [WITH ROLLUP]] 
	[LIMIT {[_offset_,] _row_count_ | _row_count_ OFFSET _offset_}] 
	[_into_option_] 
	[FOR {UPDATE | SHARE} 
		[OF _tbl_name_ [, _tbl_name_] ...] 
		[NOWAIT | SKIP LOCKED] 
	    | LOCK IN SHARE MODE] 
	[_into_option_] 
	
_into_option_: { 
	INTO OUTFILE '_file_name_' 
		[CHARACTER SET _charset_name_] 
		_export_options_ 
	| INTO DUMPFILE '_file_name_' 
	| INTO _var_name_ [, _var_name_] ... 
} 

_export_options_: 
	[{FIELDS | COLUMNS} 
		[TERMINATED BY 'string'] 
		[[OPTIONALLY] ENCLOSED BY 'char'] 
		[ESCAPED BY 'char'] 
	] 
	[LINES 
		[STARTING BY 'string'] 
		[TERMINATED BY 'string'] 
	]
```

공식 사이트에서 직접 가져온 것이다 보니 장황하게 많이 이것저것 써 놨지만, 간단하게 설명하자면 다음 표처럼 설명할 수 있다. 

`SELECT` 구문에서 특정 조건을 만족하는 데이터를 찾기 위한 `WHERE`, 조회한 결과를 정렬하거나 특정한 부분만을 확인하기 위한 `ORDER BY`, `LIMIT` 등으로 설명할 수 있다. 

| 절          | 설명                                    |
| ---------- | ------------------------------------- |
| `SELECT`   | 해당 문자열을 시작으로, 조회하기 위한 표현식과 컬럼들에 대해 정의 |
| `FROM`     | 데이터를 조회할 테이블의 이름                      |
| `WHERE`    | 조회할 데이터가 충족하는 조건                      |
| `GROUP BY` | 데이터를 특정 기준으로 묶어서 각 그룹별로 집계 연산         |
| `HAVING`   | `GROUP BY`로 묶은 데이터의 그룹에 조건 지정         |
| `ORDER BY` | 조회한 데이터를 원하는 컬럼 기준으로 정렬               |
| `LIMIT`    | 조회한 데이터에서 행의 갯수와 오프셋을 지정              |

**`SELECT` 사용 예시**
아래는 `SELECT` 구문을 사용한 예시로, 쿼리를 살펴보면 먼저 `FROM` 절을 사용해 원하는 테이블의 `uid`, `title`, `boardcontent` 데이터를 검색한다. 그리고 `WHERE` 절을 사용해 `boardcontent` 데이터에 "abc" 문자가 포함되어 있는지 검색한다. 이렇게 찾은 데이터는 `ORDER BY` 절을 사용해 `uid`를 기준으로 내림차순 정렬한 후, 5개의 행을 결과로 반환한다. 

```sql
SELECT 
	uid, title, boardcontent
FROM board
WHERE boardcontent like '%abc%'
ORDER BY uid DESC
LIMIT 5
```

---
### INSERT
**INSERT**는 데이터를 추가하는 구문으로, 다음은 [MySQL 공식 홈페이지](https://dev.mysql.com/doc/refman/8.4/en/insert.html)에서 제공하는 `INSERT` 구문의 예시이다. 

```sql
INSERT [LOW_PRIORITY | DELAYED | HIGH_PRIORITY] [IGNORE] 
	[INTO] _tbl_name_ 
	[PARTITION (_partition_name_ [, _partition_name_] ...)] 
	[(_col_name_ [, _col_name_] ...)] 
	{ {VALUES | VALUE} (_value_list_) [, (_value_list_)] ... } 
	[AS _row_alias_[(_col_alias_ [, _col_alias_] ...)]] 
	[ON DUPLICATE KEY UPDATE _assignment_list_] 
	
INSERT [LOW_PRIORITY | DELAYED | HIGH_PRIORITY] [IGNORE] 
	[INTO] _tbl_name_ 
	[PARTITION (_partition_name_ [, _partition_name_] ...)] 
	SET _assignment_list_ 
	[AS _row_alias_[(_col_alias_ [, _col_alias_] ...)]] 
	[ON DUPLICATE KEY UPDATE _assignment_list_] 
	
INSERT [LOW_PRIORITY | HIGH_PRIORITY] [IGNORE] 
	[INTO] _tbl_name_ 
	[PARTITION (_partition_name_ [, _partition_name_] ...)] 
	[(_col_name_ [, _col_name_] ...)] 
	{ SELECT ... 
		| TABLE _table_name_ 
		| VALUES _row_constructor_list_ 
	} 
	[ON DUPLICATE KEY UPDATE _assignment_list_] 
	
_value_: 
	{_expr_ | DEFAULT} 
	
_value_list_: 
	_value_ [, _value_] ... 
	
_row_constructor_list_: 
	ROW(_value_list_)[, ROW(_value_list_)][, ...]
	
_assignment_: 
	_col_name_ = 
		_value_ 
		| [_row_alias_.]_col_name_ 
		| [_tbl_name_.]_col_name_ 
		| [_row_alias_.]_col_alias_ 
		
_assignment_list_: 
	_assignment_ [, _assignment_] ...
```

이것도 `SELECT`처럼 길게 써 놨지만, 요약하자면 데이터를 추가할 테이블과 컬럼을 정의하는 `INTO`, 추가할 데이터를 정의하는 `VALUES` 등이 있다. 

| 절                 | 설명                                 |
| ----------------- | ---------------------------------- |
| `INSERT`          | 해당 문자열을 시작으로 하여 추가할 테이블과 데이터를 정의   |
| `INTO`            | 데이터를 추가할 테이블의 이름과 컬럼을 정의           |
| `SET`             | 컬럼=값 할당형으로 삽입 (MySQL 전용)           |
| `AS`              | 별칭(alias) 지정                       |
| `ON DUPLICATE...` | 유니크/PK 충돌 시 업데이트로 대체 (MySQL 전용)    |
| `VALUES`          | `INTO` 절에서 정의한 테이블의 컬럼에 명시한 데이터 추가 |

**`INSERT` 사용 예시**
다음 코드는 `INSERT` 구문을 사용한 예시로, `INTO` 절에서 추가할 테이블과 컬럼을 각각 `board`, `title`, `boardcontent`로 명시한다. 그리고 `VALUES` 절에서는 `INTO` 절에서 명시한 테이블의 각각의 컬럼에 "title 1", "content 1"과 "title 2", "content 2" 데이터를 추가한다. 이처럼, 쿼리 한 개로 두 개 이상의 데이터를 추가할 수 있다. 

```sql
INSERT
	INTO board (title, boardcontent)
	VALUES ('title 1', 'content 1'), ('title 2', 'content 2');
```

이 밖에도, 아래와 같이 서브 쿼리를 통해서 다른 테이블에 존재하는 데이터를 추가할 수 있다. 쿼리를 살펴보면, 이전과 같이 추가할 테이블과 컬럼을 각각 명시해 준 뒤, 데이터를 추가하되, `boardcontent`에는 `SELECT` 구문을 같이 사용해 `users` 테이블에 `uid` 컬럼의 값이 "admin"인 행을 찾고, 해당 행의 `upw` 데이터를 추가한다. 

이처럼, 서브쿼리를 사용하면 다른 테이블에 있는 데이터를 그대로 가져와 삽입할 수 있다. 

```sql
INSERT
	INTO board (title, boardcontent)
	VALUES ('title 1', (select upw from users where uid='admin'))
```

(참고로, SQL 쿼리에서는 대소문자를 구분하지 않아 대문자로 써도 상관없다)

![[Pasted image 20260209222542.png]]

---
### UPDATE
**UPDATE**는 데이터를 수정하는 구문으로, 다음은 [MySQL 공식 홈페이지](https://dev.mysql.com/doc/refman/8.4/en/update.html)에서 제공하는 `UPDATE` 구문의 예시이다. 

```sql
# 단일 테이블 구문
UPDATE [LOW_PRIORITY] [IGNORE] _table_reference_ 
	SET _assignment_list_ 
	[WHERE _where_condition_] 
	[ORDER BY ...] 
	[LIMIT _row_count_] 
	
_value_: 
	{_expr_ | DEFAULT} 
	
_assignment_: 
	_col_name_ = _value_ 
	
_assignment_list_: 
	_assignment_ [, _assignment_] ...
	

# 다중 테이블 구문
UPDATE [LOW_PRIORITY] [IGNORE] _table_references_ 
	SET _assignment_list_ 
	[WHERE _where_condition_]
```

단일 테이블이 복잡하기는 하지만, 실제로는 다중 테이블을 사용할 일이 많으니, 여기서는 다중 테이블을 자세하게 설명하고, 단일 테이블의 경우는 `*` 기호를 붙여 표 하단에 추가해 놓겠다.

| 절            | 설명                          |
| ------------ | --------------------------- |
| `UPDATE`     | 해당 문자열을 시작으로 하여 수정할 테이블을 정의 |
| `SET`        | 수정할 컬럼과 데이터를 정의             |
| `WHERE`      | 수정할 행이 충족하는 조건을 정의          |
| `* ORDER BY` | 지정한 순서대로 행을 업데이트            |
| `* LIMIT`    | 업데이트할 수 있는 행의 수             |

**`UPDATE` 사용 예시**
다음은 `UPDATE` 구문을 사용한 예시로, `board` 테이블의 `boardcontent` 컬럼을 "update content 2" 문자열로 수정한다. 이때 수정될 데이터의 조건은 `title` 컬럼의 값이 "title 1"인 행인 것을 `WHERE` 절을 보면서 알 수 있다. 

```sql
UPDATE board
	SET boardcontent = "update content 2"
	WHERE title = 'title 1';
```

---
### DELETE
**DELETE**는 데이터를 삭제하는 구문으로, 다음은 [MySQL 공식 홈페이지](https://dev.mysql.com/doc/refman/8.4/en/delete.html)에서 제공하는 `DELETE` 구문의 예시이다. 

```sql
DELETE [LOW_PRIORITY] [QUICK] [IGNORE] FROM _tbl_name_ [[AS] _tbl_alias_] 
	[PARTITION (_partition_name_ [, _partition_name_] ...)] 
	[WHERE _where_condition_] 
	[ORDER BY ...] 
	[LIMIT _row_count_]
```

다음은 `DELETE` 구문의 각각의 절들을 자세하게 설명한 표이다. 

| 절          | 설명                                          |
| ---------- | ------------------------------------------- |
| `DELETE`   | 해당 문자열을 시작으로 하여 삭제할 테이블을 정의                 |
| `FROM`     | 삭제할 테이블을 정의                                 |
| `AS`       | 별칭(alias) 지정                                |
| `WHERE`    | 삭제할 행이 충족하는 조건을 정의                          |
| `ORDER BY` | 지정된 순서대로 행을 삭제 (주로`LIMIT`와 함께 사용)           |
| `LIMIT`    | 삭제할 최대 행 수 정의, 단일 테이블에서만 사용 (성능 최적화 때문에 사용) |

**`DELETE` 사용 예시**
다음은 `DELETE` 구문을 쓴 예시로, `board` 테이블의 `title` 컬럼이 "title 1"인 행을 삭제한다.

```sql
DELETE FROM board
	WHERE title = 'title 1';
```

---
## SQL DML Lab
지금까지 배운 내용을 [SQL DML Lab](https://learn.dreamhack.io/labs/dbe0c2d2-0aa9-4a2d-9903-344edc12dd1f)을 통해 확인해 볼 수 있다. 

`SELECT`, `INSERT`, `UPDATE`, `DELETE` 구문을 전부 배웠지만, Lab에서 실습해보면서 동시에 저번에 실습으로 만들어 본 [[Building a Web Service#HTML & Flask를 사용해 게시판 웹 서비스 구축해보기|HTML & Flask를 사용한 웹 서비스 구축]] 부분의 *app.py* 코드를 열심해 복습하는 것이 SQL 학습에 도음이 많이 될 것이다. 

---
## 마치며
이번 강의에서는 DML과 간단한 서브 쿼리에 대해 MySQL 공식 홈페이지를 참고하면서 공부했다. 일반적으로 사용자의 입력값은 DML 구문의 일부로 사용되기 때문에 다른 질의 언어에 대해서는 다루지 않았다. 

공격자는 SQL Injection을 수행하려면 기존에 실행될 쿼리를 이해하고, 이와 동시에 특정한 조건을 생성해 또 다른 쿼리를 만들어낼 수 있어야 한다. 따라서, 앞으로 심화된 공격 기법을 이해하기 위해서는 가장 기본이 되는 SQL 쿼리를 직접 실습해보고 경험을 늘려나가는 것이 필수적이다. 

![[Pasted image 20260209230526.png]]