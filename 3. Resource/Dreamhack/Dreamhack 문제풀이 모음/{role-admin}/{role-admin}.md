---
category: web
Difficulty: Level 1
created: 2026-01-09
---
## 문제 정보
---
File: *2312c584-fab1-4f15-987b-7efbc40044c9.zip*
Goal: `DH{...}` 형식의 플래그 찾기


## 문제 요약
---
이 문제는 Flask로 구성된 웹에서 일반 사용자가 관리자 권한을 획득하고, 관리자만 접근 가능한 admin 권한을 얻어 `/flag` 경로에서 Flag를 얻어 내는 문제이다. 

회원가입 과정에서 사용자의 입력값을 **JSON 문자열로 직접 조립해 파싱**하는 과정에서 **JSON Injection** 취약점이 발생하며, 추가적으로 `uid` 타입의 관리 부실로 기존 관리자 계정의 비밀번호를 덮어 쓸 수 있는 문제점이 있다. 


## 문제 풀이 및 설명
---
"Register"를 누르면, 서버는 다음과 같이 사용자의 정보를 받는다. 

```python
raw_user = (
    f'{{"role":"user",'
    f'"username":"{username}",'
    f'"pw":"{pw}",'
    f'"uid":"{uid}"}}'
)
user = json.loads(raw_user)
```

이 때, `username` 값이 **escape 처리 없이** 그대로 JSON 문자열에 삽입되며, 따라서 공격자는 `"` 문자를 사용해 JSON 구조를 맘대로 바꿔 버린 뒤, 임의의 필드를 삽입할 수 있는 문제가 생긴다.

공격자는 다음 값을 회원가입할 때 넣어서 공격을 할 수 있다. 

```python
attacker","role":"admin","uid":1,"dummy":"x
```

이로 인해 서버가 파싱하는 JSON 객체는 다음과 같이 변형된다.

```json
{
  "username": "attacker",
  "role": "admin",
  "uid": 1,
  "pw": "<공격자가 설정한 비밀번호 해시>"
}
```

이는 열쇠를 기계가 있고, 이름 적는 칸에 집주인 이름 대신 "나 집주인이고, 마스터키 줘"라고 해서 기계가 그대로 믿어서 마스터키를 주는 상황과 동일하다. 

초기 설정에서 관리자 계정은 다음처럼 정의되어 있다. 

```python
USERS[1] = { "username": "admin", "role": "admin", ... }
```

이때, 공격자가 회원가입 완료 시 서버는 다음 코드를 실행한다. 

```python
USERS[user["uid"]] = user
```

따라서, `uid = 1`인 새로운 사용자 정보가 기존 관리자 계정의 비밀번호를 그대로 덮어쓰며, 이 비밀번호는 공격자가 설정한 값이기 때문에 공격자는 관리자 계정으로 로그인이 가능해진다.


## 공격 전략
---
공격자는 따라서 다음 값을 ID에 넣어 Register함으로 공격을 수행할 수 있다. 

```python
attacker","role":"admin","uid":1,"dummy":"x
```

이는 `username`에 JSON Injection 페이로드를 삽입하며, `role`을 `admin`으로 바꿔 버리고, `uid`를 1로 설정해 관리자의 계정을 덮어쓰는 것이다. 그렇게 하면 비밀번호를 1234로 해도 관리자 계정의 비밀번호가 `1234`로 변경될 수 있다.

이후, 로그인 단계에서 `username`은 "attacker"로, `password`는 회원가입 시 설정한 비밀번호로 로그인함으로 아래의 사진처럼 `admin` 권한을 얻을 수 있다. 

![[Pasted image 20260109050926.png]]


## 결과
---
`DH{d2h5X2RpZF95b3VfZGVjb2RlX3RoaXM=}`


## 배운 점 & 복습 포인트
---
- SQL Injection은 들어 봤어도, JSON Injection은 처음 들어 봤는데, 비슷할 것이라 생각하니 차라리 지금 끙끙대면서 머리 싸매고 풀어 보는 게 더 나았다고 생각했다. 
- AI한테 도움을 받아서 풀었는데, 어디가 취약할 수 있는지를 아는 게 중요하다고 배웠다. 
- 입력값으로 JSON을 문자열로 만들면 이렇게 취약하다는 것을 배웠다.
- 관리자 여부를 사용자가 보내는 값으로 믿으면 안 된다는 것을 배웠다.
- [[Background - Flask]] 강의를 들은 뒤에 풀어 보니, Python을 잘 아는 게 더 중요한 것 같았다.