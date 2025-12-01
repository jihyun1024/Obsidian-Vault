웹 사이트에서 사용되는 이미지를 [[WireShark]]로 추출해 보는 실습을 해 보자. 실습 파일은 Wireshark 폴더 내부에 [[kookminuniv_capture.pcapng]]로 있다.

## Wireshark 설정
Wireshark에서 SSLKEYLOGFILE 환경변수를 다음과 같이 설정한다. 

1. 제어판 - 모든 제어판 항목 - 고급 시스템 설정 - 환경 변수 - 시스템 변수 - 새로 만들기
2. 변수 이름: SSLKEYLOGFILE로 설정, 변수 값: 원하는 경로 (보통 `C:\바탕 화면\sslkey.log`로 알아보기 쉽게 설정)
3. Wireshark 실행
4. 편집 - 설정 - Protocol - TLS
5. (Pre)-Master-Secret log filename 부분에 환경 변수 값이 저장된 경로 입력
6. 설정 후 Wireshark와 인터넷 브라우저 전부 종료 이후 PC 재시작

## 패킷 캡처
Wireshark를 킨 채로 브라우저를 실행한 후 HTTPS를 지원하는 페이지에 접속하면, 이전과 달리 복호화가 성공적으로 완료된 패킷을 확인할 수 있다. ![[Pasted image 20251202042817.png]]

이렇게 복호화가 정상적으로 되는 이유는, 클라이언트가 세션 키 = 대칭키를 외부 파일에 기록해 두기 때문으로, 다음과 같이 요약할 수 있다. 

- TLS에서는 실제 데이터 내용을 대칭키로 암호화
- 해당 대칭키는 TLS handshake 과정에서 비밀로 만들어짐
- `SSLKEYLOGFILE`을 생성하면 Chrome/Firefox 등의 브라우저가 해당 세션 키 생성 과정을 그대로 파일에 적어줌
	- 파일에는 TLS 1.3의 경우, 다음 사진의 정보가 담김![[Pasted image 20251202043414.png]]
	- 이 때 `CLIENT_HANDSHAKE_TRAFFIC_SECRET` 등은 그 세션에서 생성한 대칭키를 계산할 수 있는 재료들로, 원래는 비밀이었어야 할 정보들
- Wireshark는 이 키를 읽어서 해당 TLS 스트림을 바로바로 복호화할 수 있음

## IP 주소 확인 및 데이터 덤프
Command 창을 열어서 `nslookup (웹사이트 도메인)` 명령어를 타이핑해 해당 사이트의 IP 주소를 얻는다. 

![[Pasted image 20251202044437.png]]
해당 예시에서는 `kookmin.ac.kr`의 IP 주소는 113.198.215.98이 된다. (이 예시에서는 `ecampus.kookmin.ac.kr`로 대신 실행했다)

이후 Wireshark에서 해당 IP주소로 송수신 된 것 중에서 HTTP 트래픽을 찾아 `(JPEG JFIF image)`라고 되어 있는 것을 찾는다. 
![[Pasted image 20251202045039.png]]

이 때, JPG의 파일 시그니처는 [[JPG의 구조]] 문서를 보면 `FF D8`로 시작하기 때문에, 해당 시그니처를 찾아 복사를 수행한다. 

복사는 왼쪽 창의 `JPEG File Interchange Format`을 우클릭해 `복사 - 16진수 스트림으로 복사`
를 선택하고 HxD에서 새로운 파일을 만들어 붙여넣기 한다. 

이후, 확장자에 맞게 JPG 또는 PNG로 확장자를 변경하고 저장하면 다음 세 개의 사진처럼 정상적으로 사진이 나오게 된다.
![[Wireshark_test_1.jpg|300]]![[Wireshark_test_2.jpg|300]]]
![[무제1.jpg|300]]

## Server Private Key만으로 TLS 복호화가 되지 않는 이유
TLS 1.3 또는 TLS 1.2 ECDHE에서는 서버의 대칭키만으로는 Client-Server shared secret을 만들 수 없는데, 그 이유는 [[양자보안_6주차_수요일]]에서 서버의 비밀값인 $d_A$를 알아내도 일회성이기 때문에 복원이 불가능하기 때문이다. 

따라서 Wireshark에서는 server private key 추가만으로는 TLS 1.3이 복호화 되지 않고, `SSLKEYLOGFILE`을 사용하는 것만이 유일하게 정상적으로 복호화를 수행할 수 있다. 

## Chrome/Firefox 등이 SSLKEYLOGFILE을 지원하는 이유
이러한 위험성에도 불구하고, Chrome이나 Firefox 등의 브라우저에서는 왜 SSLKEYLOGFILE을 지원할까? 

사실, SSLKEYLOGFILE은 원래 개발자 용도로 있던 것으로, HTTPS 트래픽 분석, TLS handshake 디버깅, 브라우저 내부 TLS 구현 테스트 등의 목적 때문에 있는 것이다. 

따라서, 암호 자체는 여전히 안전하다고 볼 수 있다.