---
tags:
  - Dreamhack
  - Easy
  - Skill_Path
---
## 학습 목표
오늘날 웹은 우리의 삶과 밀접한 핵심 기술 중 하나이다. 사람들은 웹을 사용해 필요한 정보를 검색하고, 게임을 하며, 온라인으로 쇼핑을 하기도 한다. 이러한 웹 서비스들은 관련 기술이 발전하며 그 종류 또한 다양해졌는데, 웹을 기반으로 한 금융, 길 찾기 서비스, 쇼핑, 협업, 게임 등의 서비스들은 이제 현대인의 일상 속 한 부분을 차지하고 있다. 

한편으로 웹에 대한 의존성이 높아진 만큼, 웹 서비스를 안전하게 구현하고 관리하는 것이 매우 중요한 과제가 되었다. 예를 들어, 2025년 말 AWS 서비스의 다운으로 Clash of Clan, Fall Guys, Steam, 배틀그라운드 등 다양한 게임이 접속 장애를 겪게 된 사례가 있다. 

이번 Path는 안전한 웹 서비스의 구현을 위해 반드시 알아둬야 할 웹 해킹의 기초적인 지식과 기술을 전달하는 것을 목표로 한다. 해킹에 대한 막연한 호기심이건, 웹 해킹에 대해 전체적으로 알아가고 싶던 뭐든 좋으니, 해당 Path를 들으면서 관련 자료도 참고하며 열심히 공부해 보자.

**(이 문서의 내용들만 보지 말고, [Dreamhack 강의 링크](https://dreamhack.io/lecture/paths/web-hacking-fundamental)에 있는 Lab이나 Quiz도 같이 보며 학습하자!)**

---
## 배우는 내용
- HTTP/HTTPS와 웹 브라우저의 구조 등 웹의 기본 개념
- 웹 해킹의 기초부터 심화까지 단계적인 학습
- XSS, CSRF 등 Client-Side 취약점 분석 및 공격 실습
- SQL Injection, NoSQL Injection, SSRF 등 Server-Side 취약점 분석 및 공격 실습
- Command Injection, 파일 업로드/다운로드 등 다양한 실전 취약점 실습
- 워게임 문제 풀이를 통한 실전 감각 및 문제 해결 능력 실습

---
## Unit 구성
### 웹 기초 지식
[[Background - Web & HTTPS]]
[[Background - Web Browser]]
[[Tools - Browser DevTools]]
[[Exercise - devtools-sources]]
### Cookie & Session
[[Background - Cookie & Session]]
[[Exercise - Cookie]]
[[Exercise - Cookie & Session]]
[[Mitigration - Same Origin Policy]]
### Cross-Site Scripting (XSS)
[[ClientSide - XSS]]
[[Exercise - XSS]]
[[Exercise - XSS_2]]
### Cross-Site Request Forgery (CSRF)
[[ClientSide - CSRF]]
[[Exercise - CSRF]]
[[Exercise - CSRF_2]]
### SQL Injection
[[Background - 데이터베이스]]
[[Background - Relational DBMS]]
[[ServerSide - SQL Injection]]
[[Background - SQL DML]]
[[Background - SQL Features]]
[[Exercise - SQL Injection]]
[[Exercise - Blind SQL Injection]]
### NoSQL Injection
[[Background - Non Relational DBMS]]
[[ServerSide - NoSQL Injection]]
[[Exercise - NoSQL Injection]]
### Command Injection - Web Servers
[[ServerSide - Command Injection]]
[[Exercise - Command Injection]]
### File Vulnerability - Web Servers
[[ServerSide - File Vulnerability]]
[[Exercise - File Vulnerability]]
[[Exercise - File Vulnerability_2]]
### Server-Side Request Forgery (SSRF)
[[ServerSide - SSRF]]
[[Exercise - SSRF]]

---
## 선수 지식
- 기본적인 컴퓨터 사용 능력
- 네트워크에 대한 기초적인 이해
- HTML 및 JavaScript의 기초적인 이해
- Python 기본 문법과 Flask 프레임워크 학습 경험

---
## 관련 문서
- [[보안네트워크 프로그래밍]]
- [[Building a Web Service]]
- [[Background - Flask]]
- [[Background - HTML]]
- [[Background - Web & HTTPS]]
- [[Python]]