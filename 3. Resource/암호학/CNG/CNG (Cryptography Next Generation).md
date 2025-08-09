[[랜섬웨어 제작 (2025)|랜섬웨어 제작]] 활동에서 공부했던 CNG API에 대해 알아볼 것이다. 

CNG(Cryptography Next Generation)는 Microsoft에서 개발한 차세대 암호화 API로, Windows 플랫폼에서 보안 기능을 구현할 때 사용하는 기술이다. 
기존의 CryptoAPI를 대체하기 위해 만들었으며, 다양한 암호화 알고리즘을 지원한다. 
![[Pasted image 20250809193724.png]]
### 주요 구성 요소
* BCrypt API
	* 용도: [[AES with CNG|대칭키 암호화]], [[RSA with CNG|비대칭키 암호화]], 해시, [[RNG with CNG|난수 생성]] 등 순수 암호 연산
	* DLL: `bcrypt.dll`
	* 예시 함수: [BCryptOpenAlgorithmProvider 함수](https://learn.microsoft.com/ko-kr/windows/win32/api/bcrypt/nf-bcrypt-bcryptopenalgorithmprovider), [BCryptCloseAlgorithmProvider 함수](https://learn.microsoft.com/ko-kr/windows/win32/api/bcrypt/nf-bcrypt-bcryptclosealgorithmprovider)
* NCrypt API
	* 용도: 키 저장 및 관리 (KSP, Key Storage Provider) 기능
	* DLL: `ncrypt.dll`
	* 예시 기능
		* Windows 인증서 저장소 또는 HSM(Hardware Security Module)에 키 저장
		* 키 가져오기/내보내기
		* 암호화 키에 대한 접근 제어
* PCP API
	* 용도: TPM(Trusted Platform Module)과 연동하는 Provider (TPM 키 생성, 서명, 인증 등)
	* DLL: `ncrypt.dll` 내부

### 특징
1. 모듈화된 구조: 알고리즘, 키 저장소, 하드웨어 가속을 플러그인 형태로 추가 가능
2. FIPS 140-2/3 준수: 보안 인증 표준을 충족하도록 설계
3. 최신 알고리즘 지원: ECC, SHA-2, AES/GCM, SHA-3 등 현대 암호 표준 지원
4. 커널 모드 지원: OS 레벨 암호화와 애플리케이션 레벨 암호화를 전부 지원
5. 기존 보안 정책과의 통합: 필요한 경우 기존의 CryptoAPI와 함께 사용 가능

![[Pasted image 20250809194514.png]]

### Typical CNG Programming
CNG API를 사용한 프로그래밍의 과정
1. Algorithm Provider를 연다.
2. Algorithm Properties를 Get하거나 Set한다.
3. Key를 만들거나 가져온다. 
4. 암호 연산을 수행한다.
5. Algorithm Provider를 닫는다. 
![[Pasted image 20250809194723.png]]![[Pasted image 20250809194822.png]]