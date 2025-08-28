---
tags:
  - Dashboard
---

모든 랜섬웨어는 파일을 암호화해서 사용자에게 복호화 키를 볼모로 비트코인 등의 경제적인 대가를 요구한다. 따라서, 랜섬웨어에는 암호화 알고리즘이 들어 있으며, 금전을 받아서 이를 복호화 하기 위한 알고리즘도 동시에 들어 있다.
따라서, 우리가 알고자 하는 것은 어떤 알고리즘이 사용되었는지, 어떤 순서로 파일이 암호화되는지, 그리고 최종적으로 어떻게 복호화를 수행하는지 등을 분석하면 된다. 
그 전에 먼저 Wi-Fi 해제, 공유 폴더 끄기 등의 설정을 VMware에서 해서 호스트 머신이 감염되는 불상사는 피하도록 하자.

---
### 정적 분석 및 동적 분석
[[x64dbg로 Ragnar Locker 코드 흐름 파악 (동적 분석)]]
[[Ragnar Locker 파일 암호화 알고리즘 파악]]
[[Ragnar Locker 랜섬웨어 복호화]]

### Feedback
[1차 피드백]
1. 발표 할 때 목소리 크게 하고 자신 있게 하면 좋겠다. 
2. 인턴 중 가장 열심히 하고 가장 잘 하고 있다 + 처음보다 발표가 많이 늘었다. 
3. Salsa 시리즈 구글링해서 더 자세히 찾아보자.
4. 파일 암호화에 사용된 알고리즘에 대해 분석하자. 
5. 랜섬웨어 복호화가 가능한지, 불가능한지 알아보자.
	1. 가능하다면 어떻게 복호화 할 수 있는지, 그리고 그걸 바탕으로 C나 Python 코드를 짜자.
	2. 불가능하다면 왜 불가능한지 이유를 입증하자. 

[2차 피드백]
1. 발표 자료가 처음 Retro Steganography 발표할 때보다 많이 발전했다.
	1. 발표 자료에서 슬라이드 하나하나가 어떤 걸 말하고자 하는지 한 눈에 보인다. 
	2. 딱히 보완해야 할 피드백은 없다.
2. 다음은 JPG 구조 공부, F5 알고리즘 공부, JPG 스테가노그래피 분석 등등 할 것 같다.
	1. 먼저 JPG 구조에 대해 공부해야 할 것 같다.
3. 이대로 잘 풀린다면 4학년 때 연구실 프로젝트에 같이 참여할 수 있을 것이다.

---
### 분석 결과
참고자료와 동적 분석으로 파악한 사실에 의하면, Ragnar Locker 랜섬웨어는 다음과 같이 진행된다. 
![[Pasted image 20250828133418.png]]
여기서, 제외 대상인 언어는 Azerbaijani, Armenian, Belorussian, Kazakh, Kyrgyz, Moldavian, Tajik, Russian, Turkmen, Uzbek, Ukrainian, Georgian 총 12개이다. 

이 때 랜섬웨어 실행, 운영체제 실행과 관련된 파일 또는 폴더를 제외한 모든 파일을 암호화하며, 암호화 제외 대상 목록은 다음과 같다. 
![[Pasted image 20250729174153.png]]
### 참고자료
[키 재사용 공격을 통한 Ragnar Locker 랜섬웨어 감염 파일 복호화 및 활용 방안 연구](https://www.dbpia.co.kr/journal/articleDetail?nodeId=NODE10551540)
['It's very scary': Small banks quietly hit by ransomware attacks.](https://research.ebsco.com/c/eg7l4x/search/details/62ppjgwwxv?db=bth)
[스트림 암호 기반 랜섬웨어에 대한 기술적 분석 동향](https://www.dbpia.co.kr/journal/articleDetail?nodeId=NODE11077406)
