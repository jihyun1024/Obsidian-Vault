## 들어가며
---
### 서론
---
이 강의에서는 [session-basic](https://dreamhack.io/wargame/challenges/409) 워게임을 실습한다. 

본 문제의 서버도 [[Exercise - Cookie]]처럼 Python Flask 프레임워크를 통해 구현되었으며, Python을 기반으로 만들어진 문제이기 때문에 Python에 대해 알고 있어야 한다. 


### 문제 목표 및 기능 요약
---
해당 문제의 목표는 관리자 권한을 획득해 플래그를 획득하는 것으로, 문제에서는 다음의 두 페이지를 제공한다. 

- `/`: 사용자의 username을 출력하고 관리자 계정인지 확인한다.
- `/admin`: 관리자가 사용자의 세션 정보를 확인한다.


## 웹 서비스 분석
---
### 엔드포인트: `/`
---
아래 코드는 index page를 구성하는 코드이다. 해당 코드에서는 세션을 통해 사용자를 식별한다. 먼저 cookie의 `sessionid` 값을 통해 `session_storage`에서 해당 Session ID의 `username`을 조회한다. 만약 `username`이 "admin"일 경우, 플래그를 출력한다. 

```python
@app.route('/') # / 페이지 라우팅 
def index():
    session_id = request.cookies.get('sessionid', None) # 쿠키에서 sessionid 조회
    try:
        username = session_storage[session_id] # session_storage에서 해당 sessionid를 통해 username 조회
    except KeyError:
        return render_template('index.html')

    return render_template('index.html', text=f'Hello {username}, {"flag is " + FLAG if username == "admin" else "you are not admin"}')
```


### 엔드포인트: `/admin`
---
admin 페이지 코드는 관리자 페이지를 구성하는 코드이다. `developer's note` 아래의 주석처리된 코드는 index page와 비슷한 구조로 세션을 통해 현재 접속한 사용자가 admin인지 확인하려 했음을 볼 수 있다. 

**admin 세션 생성**은 서비스 실행 시 `os.urandom(32).hex()`를 통한 무작위 값 생성을 통해 `username`이 admin인 세션 정보를 session storage에 생성한다. 즉, 해당 session_storage에 접근해 정보를 조회할 수만 있다면 무작위 값을 찍지 않고도 곧바로 admin 계정에 해당하는 Session ID 값을 획득할 수 있다. 

```python
# admin 페이지 코드
@app.route('/admin')
def admin():
    # developer's note: review below commented code and uncomment it (TODO)

    #session_id = request.cookies.get('sessionid', None)
    #username = session_storage[session_id] session_storage에 저장된 username을 불러옴
    #if username != 'admin': # username이 admin인지 확인
    #    return render_template('index.html')
      
    return session_storage
    
    
# admin 세션 생성
if __name__ == '__main__':
    import os
    # create admin sessionid and save it to our storage
    # and also you cannot reveal admin's sesseionid by brute forcing!!! haha
    session_storage[os.urandom(32).hex()] = 'admin' # username이 admin인 Session ID를 무작위로 생성
    print(session_storage)
    app.run(host='0.0.0.0', port=8000)
```


### 취약점 분석
---
admin page 코드를 다시 살펴보면, 전체 세션 정보가 포함된 `session_storage`는 `username`이 admin인 관리자만 조회할 수 있도록 되어 있지만, 정작 주석처리 되어있어 인증을 거치지 않고 누구나 Session Storage를 조회할 수 있다. 


## 익스플로잇
---
먼저, 링크의 뒤에 `/admin`을 입력해 admin page에서 **세션 정보 조회**처럼 현재 접속된 모든 사용자의 Session ID와 `username`을 조회한다. 

![[Pasted image 20260103220803.png]]

해당 이미지에서 Session ID를 복사해 cookie의 `sessionid`의 값을 생성하며, 이는 웹 브라우저의 쿠키 변조처럼, 개발자 도구를 사용해 Session 탭의 Cookies 탭으로 들어가 `Name`에는 `sessionid`를, `Value`에는 아까 복사한 값을 붙여넣어 저장하면 쿠키의 정보를 수정할 수 있다. 

![[Pasted image 20260103221123.png]]

admin의 Session ID가 적용된 상태에서 페이지를 새로고침하면 플래그를 획득할 수 있다. 

![[Pasted image 20260103221116.png]]


## 마치며
---
이번 강의에서는 제거되지 않은 디버그 코드를 통한 **세션 하이재킹(Session Hijacking)** 을 통해, 자신 이외의 사용자에 대한 Session ID를 알아내 플래그를 획득할 수 있었다. 실습에서는 해당 취약점들을 연계해 다른 계정의 권한을 획득할 수 있었다. 

이 취약점을 방지하려면, 기본적으로 배포 전에 **디버그 코드가 남아있는지 검사**함과 동시에, "developer's note"나 "TODO" 등의 문구를 찾아보며 주석된 메시지와 코드들을 점검하는 습관을 들이는 것이 좋다.

Session Hijacking 공격에 대해서는 **세션 타임아웃**을 적용해 해결할 수 있다. 세션 타임아웃이란, 일정 주기마다 Session ID를 재발급해 정상 로그인한 사용자는 장기간 세션을 유지할 수 있는 반면, 탈취한 Session ID로 로그인한 공격자는 일정 시간 이후 더 이상 해당 인증이 유효하지 않게 되어 안전한 서비스를 구현할 수 있다. 

![[Pasted image 20260103221752.png]]
