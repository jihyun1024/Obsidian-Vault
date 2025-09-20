## 요약
## I. 서론
비밀번호 관리자, 또는 패스워드 매니저(Password manager)는 컴퓨터나 모바일에 애플리케이션이나 브라우저 확장으로 설치되어 사용자가 온라인 결제나 웹에서 사용하는 비밀번호를 안전하게 저장하고 효율적으로 관리할 수 있도록 하는 응용 프로그램이다[^1]. 현대 디지털 환경에서 사용자는 수십 개의 비밀번호를 관리해야 하며, 이 과정에서 비밀번호 재사용 혹은 '1q2w3e4r', '123456' 등 흔한 비밀번호 패턴 사용으로 만들어진 비밀번호 등 다양한 취약점으로 인한 공격으로 사용자의 비밀 정보가 유출될 경우 다른 민감한 정보 또한 쉽게 노출될 위험이 있다[^2][^3]. 실제로 2024년 100억 개에 해당하는 비밀번호가 일반 텍스트 형태로 유출되었으며 이 중에는 재사용된 비밀번호의 비중이 높아 이메일 주소 및 기타 개인 정보가 포함된 다른 유출된 데이터와 결합될 경우 금융 사기, 신원 도용 등으로 이어질 수 있는 사례가 있었다[^4]. 이와 같은 비밀번호 유출을 방지하기 위해 사용자의 편리함을 보장하는 동시에 안전하게 비밀번호를 관리할 수 있는 비밀번호 관리자가 필요하다.
그러나 모든 비밀번호가 한 곳에 저장되어 관리되는 특성상 비밀번호 관리자는 그 자체로 공격의 표적이 될 수 있다. 실제로 2022년 상용 비밀번호 관리 애플리케이션이 공격의 표적이 되어 500만 달러 상당의 암호화폐를 도난당한 사례가 존재하며, 이는 사용자의 다른 계정에 대한 위협으로 다가올 수 있다[^5]. 따라서 비밀번호 관리자에 대한 보안 구조와 암호화 방식에 대한 연구가 중요하다. 
본 논문에서는 사용자의 비밀번호를 암호화하여 데이터베이스에 저장하는 비밀번호 관리 애플리케이션 Proton Pass의 아티팩트를 분석하고 이어서 암호화된 컨텐츠를 복호화 한다. 본 논문의 구성은 다음과 같다. 2장에서는 Proton Pass 애플리케이션과 관련된 배경 지식과 선행 연구를 정리한다. 3장에서는 Proton Pass의 주요 아티팩트를 설명하고 데이터베이스 내의 암호화된 컨텐츠에 대한 복호화를 수행한다. 마지막 4장은 결론으로 마무리한다. 분석 환경은 [표 1]과 같다. 

[표 1] Information of Analysis Environment

| Softwares   | Name and Version                  |
| ----------- | --------------------------------- |
| Application | Proton Pass ver. 1.32.7           |
| Decompiler  | Jadx ver. 1.5.1                   |
| DB Browser  | DB Browser for SQLite ver. 3.13.1 |
| Hex Viewer  | HxD Editor ver. 2.5.0.0           |

## II. 배경 지식 및 관련 연구
### 2.1 배경 지식
Proton Pass는 사용자 비밀번호와 같은 민감 데이터를 안전하게 저장하기 위해 Android 보안 저장소 구조를 활용한다. Android 애플리케이션은 통상 SharedPreferences를 통해 설정값이나 간단한 사용자 데이터를 Key-Value 형태로 저장하지만, 기본 SharedPreferences는 평문 저장으로 인해 루팅-포렌식 환경에서 유출 위험이 있다. 이를 보완하기 위해 Proton Pass는 EncryptedSharedPreferences를 사용한다. 
EncryptedSharedPreferences는 AndroidX Security 라이브러리의 구현체로, Key는 AES-256-SIV, Value는 AES-256-GCM으로 암호화하여 저장한다. 암호화에 사용되는 Master Key는 Android Keystore에서 관리되며 애플리케이션이 키의 평문을 직접 열람할 수 없도록 설계되어 있다. 또한 민감 데이터(예: vault 내용)는 디바이스 내에서만 복호화되며, 서버에는 암호화된 형태로만 전송·저장된다. 이러한 구조는 포렌식적 접근이나 단순 파일 추출만으로는 데이터 복호화가 어렵도록 보안성을 높인다. 

### 2.2 관련 연구
비밀번호 관리자 애플리케이션에 대한 보안 연구 또한 다수 진행되었다. Li 등은 웹 기반 비밀번호 관리자를 대상으로 자동 완성 기능과 세션 관리의 취약점을 분석하였으며, 다양한 보안 결함을 실험적으로 제시하였다[^6]. Oesch 등은 13종의 상용 비밀번호 관리자를 대상으로 비밀번호 생성, 저장, 자동 입력(autofill) 기능을 평가하여 보안성과 사용성의 한계를 도출하였다[^7]. 또한 Oesch 등은 iOS 및 Android 환경의 autofill 프레임워크를 분석하여 모바일 환경에서 발생할 수 있는 피싱과 데이터 유출 위험을 보고하였다[^8]. 본 연구는 이러한 선행연구의 관점(비밀번호 생성·저장·자동입력의 보안성 평가)을 확장하여, Proton Pass라는 보안 지향 비밀번호 관리자의 암호화 저장 구조와 키 관리 메커니즘을 분석한다.

## III. Proton Pass 복호화 및 아티팩트 분석
### 3.1 사용자 아티팩트 분석

### 3.2 데이터베이스 내 컨텐츠 복호화 방안

## IV. 결론

## 참고문헌
[^1]: Wikipedia, "Password manager", https://en.wikipedia.org/wiki/Password_manager
[^2]: 조선일보, ""이러니 털리지" 세계서 가장 흔한 비밀번호는 '123456'...한국은?", https://www.chosun.com/economy/tech_it/2024/11/18/D3B326KQ5VDFJP4UVD7XFOCCGE/
[^3]: 박소희, Yazmyradov Serdar, and 이훈재, "국내에서 많이 사용되는 비밀번호 유형과 보안 동향," 한국컴퓨터정보학회 학술발표논문집 , vol. 32, no. 2, pp. 253-255, 2024.
[^4]: McAfee, "RockYou2024: Unpacking the Largest Password Leak in History", https://www.mcafee.com/blogs/internet-security/rockyou2024-unpacking-the-largest-password-leak-in-history/
[^5]: BLEEPINGCOMPUTER, "US seizes $23 million in crypto linked to LastPass breaches", https://www.bleepingcomputer.com/news/security/us-seizes-23-million-in-crypto-stolen-via-password-manager-breach/
[^6]: Li, Z., He, W., Akhawe, D., & Song, D. (2014). _The Emperor’s New Password Manager: Security Analysis of Web-based Password Managers_. In Proceedings of the 23rd USENIX Security Symposium, pp. 465–479.
[^7]: Oesch, T., Ruoti, S., et al. (2019). _That Was Then, This Is Now: A Security Evaluation of Password Generation, Storage, and Autofill in Thirteen Password Managers_. arXiv:1908.03296.  
[^8]: Oesch, T., Gautam, A., & Ruoti, S. (2021). _The Emperor’s New Autofill Framework: A Security Analysis of Autofill on iOS and Android_. arXiv:2104.10017.