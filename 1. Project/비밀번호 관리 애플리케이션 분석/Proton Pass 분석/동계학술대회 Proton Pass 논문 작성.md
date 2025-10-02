## 요약
비밀번호 관리 애플리케이션은 사용자의 다양한 정보를 저장하고 있다. 이러한 점은 범죄 수사에서 중요한 증거가 될 수 있으므로 비밀번호 관리 애플리케이션에 대한 보안 구조와 암호화 방식에 대한 연구가 필요하다. 본 연구에서는 Proton Pass 애플리케이션을 분석하였다. 분석 결과 사용자의 정보는 암호화되어 저장되며 암호화에 사용된 키는 Android Keystore에서 관리하는 Master Key로 암호화되어 파일 형태로 저장된다. 본 연구에서는 복호화한 키를 이용해 암호화된 데이터를 복호화하고 이를 바탕으로 사용자 아티팩트 분석을 수행한다.
## I. 서론
현대의 디지털 환경에서 사용자는 여러 웹 사이트에서 사용하는 비밀번호를 전부 관리해야 한다. 비밀번호 관리의 어려움으로 비밀번호를 재사용하거나 '1q2w3e4r', '0000' 등 보안 강도가 약한 비밀번호를 사용하는 경우가 많다[^1]. 이는 사용자의 정보가 유출될 경우 다른 민감한 정보 또한 쉽게 노출되는 위험성을 가진다[^2].

이러한 보안 위협을 최소화하기 위해 패스워드 매니저(Password manager)를 사용하여 계정 정보를 안전하게 보호해야 한다.

패스워드 매니저 애플리케이션은 PC나 모바일 환경에서 사용자의 비밀번호를 안전하고 효율적으로 관리하는 응용 프로그램이다[^3].

패스워드 매니저의 특성상 다양한 웹사이트와 애플리케이션, 시스템에 대한 로그인 정보를 저장한다. 이에 더해 사용자의 계좌 정보, 신분 정보, 메모 등 다양한 정보를 추가로 저장한다. 이를 분석하면 디지털 포렌식 수사 진행 시 사용자의 행위와 계정에 대한 정보를 획득할 수 있다[^4]. 해당 정보는 범죄 수사에서 특정 개인의 식별과 자산 추적에 중요한 단서가 될 수 있어 이에 대한 보안 구조와 암호화 방식에 대한 연구가 필요하다.

본 연구에서는 패스워드 매니저 애플리케이션 Proton Pass v1.32.7을 Android 환경에서 분석하였다.

본 연구의 구성은 다음과 같다. 2장은 Proton Pass 애플리케이션과 관련된 배경 지식과 선행 연구를 정리한다. 3장은 사용자의 비밀 정보를 암호화하는 키인 pass.key 파일에 대한 복호화와 암호화된 정보에 대한 복호화를 수행한다. 4장은 Proton Pass의 주요 아티팩트를 분석한다. 마지막 5장은 결론으로 마무리한다.

## II. 배경 지식 및 관련 연구
### 2.1 배경 지식
Proton Pass는 사용자 비밀번호와 같은 민감 데이터를 데이터베이스에 암호화하여 저장하며, 이때 사용되는 키는 Android Keystore를 통해 관리된다.

Android Keystore는 Android 기기에서 키의 무단 사용을 방지하기 위해 키 저장소를 사용하여 앱에서 키를 생성하거나 가져올 때 키의 승인된 사용을 지정할 수 있다[^5]. Keystore에 저장된 키는 키 사용 시기와 방법을 제한하여 앱의 암호화 작업에 대한 보안을 강화한다.

### 2.2 관련 연구
패스워드 매니저 애플리케이션에 관한 연구는 지속적으로 이루어지고 있다. 남기훈 외 4인은 패스워드 매니저의 구조적 취약점을 바탕으로 사용자 계정 탈취 방안에 대해 연구하였다[^6]. 김수린 외 1인은 웹 및 로컬 기반 패스워드 매니저의 저장 방법 및 발생 가능한 취약점에 대한 연구를 진행하였다[^7]. 박준성 외 1인은 크롬 브라우저의 구글 패스워드 매니저의 암복호화 과정을 분석하고 복구도구를 제안하였다[^8]. 김한결 외 2인은 Android 환경에서의 패스워드 매니저 애플리케이션 4종의 암호화 알고리즘 및 취약점 보완 방안에 대해 연구하였다[^9].

## III. Proton Pass 애플리케이션의 데이터베이스 컨텐츠 복호화 방안
Proton Pass의 사용자 아티팩트는 /data/data/proton.android.pass/databases 경로의 db-passkey에 저장된다. db-passkey 파일은 SQLite 포맷으로 저장되어 있으며 내부에는 암호화된 컨텐츠가 포함되어 있다. 이 컨텐츠를 복호화하기 위해 두 단계의 복호화 과정이 필요하다.

Android Keystore에서 추출한 키를 사용하여 pass.key 파일을 복호화한다. 이후 복호화된 pass.key를 사용하여 db-passkey 파일의 컨텐츠를 최종적으로 복호화할 수 있다(그림 1).
![[Pasted image 20251002095716.png]]
(그림 1) Data Structure of Proton Pass

### 3.1 pass.key 복호화 방안
암호화된 비밀번호를 복호화하기 위한 DEK (Data Encryption Key)[^10] 생성에는 /data/data/proton.android.pass 경로에 60-byte로 저장되어 있는 pass.key 파일이 사용된다(그림 2). pass.key 파일이 존재하지 않을 경우, 32-byte 난수 배열을 생성한다.
![[Pasted image 20250924035829.png]]
(그림 2) pass.key file

암호화된 DEK인 pass.key 파일은 AES256/GCM/NoPadding 알고리즘을 사용하여 복호화할 수 있다. IV는 pass.key의 상위 12-byte를 사용하며 Tag는 pass.key의 하위 16-byte를 사용한다(그림 3).
![[Pasted image 20251002100337.png]]
(그림 3) Data Structure of pass.key File

pass.key 복호화에 사용되는 KEK(Key Encryption Key)[^11]는 Android Keystore를 통해 관리된다. KEK는 (그림 4)와 같이 /data/misc/keystore/user_X 디렉터리의 USRID_USRPKEY__me_proton_core_data_crypto_master_key 파일에 32-byte 평문으로 저장되어 있다.
![[Pasted image 20251002100430.png]]
(그림 4) Android Keystore of Proton Pass

복호화 과정을 정리하면 (그림 5)와 같다.
![[Pasted image 20250924050040.png]]
(그림 5) DEK Decryption Process

### 3.2 암호화된 데이터베이스 컨텐츠 복호화 방안
암호화된 컨텐츠는 AES256/GCM/NoPadding 알고리즘을 사용하여 복호화할 수 있다.

ItemEntity 테이블의 encrypted_title과 encrypted_note 컬럼의 경우 암호화된 컨텐츠를 Base64 디코딩하여 데이터로 정의한다. encrypted_content 컬럼의 경우 암호화된 컨텐츠가 16진수 문자열로 저장되어 있으며, 이를 바이트 객체로 변환하여 데이터로 정의한다. IV는 데이터의 상위 12-byte를 사용하며 키는 복호화된 pass.key를 사용한다. 복호화할 데이터는 IV에 연접되어 있으며, 하위 16-byte까지 사용한다. Tag는 데이터의 하위 16-byte를 사용한다. 해당 과정을 정리하면 (그림 6)과 같다.
![[Pasted image 20250924051949.png]]
(그림 6) Content Decryption Process

## IV. Proton Pass 데이터베이스 아티팩트 분석
사용자에 대한 정보는 db-passkey 파일의 AccountEntity 테이블에 저장된다. 테이블에는 사용자 ID와 Email 등이 저장되어 있다. 주요 아티팩트를 정리한 내용은 [표 1]과 같다.

[표 1] Artifacts of User Data

| Table         | Column            | Content                                  |
| ------------- | ----------------- | ---------------------------------------- |
| AccountEntity | userId            | User ID                                  |
|               | username          | User Name                                |
|               | email             | User Email                               |

암호화된 데이터는 ItemEntity 테이블에 저장된다. 테이블에는 사용자 ID와 생성 시각, 제목, 비밀번호 등이 저장되어 있다. 주요 아티팩트를 정리한 내용은 [표 2]와 같다.

[표 2] Artifacts of Password Data

| Table      | Column            | Content                    |
| ---------- | ----------------- | -------------------------- |
| ItemEntity | id                | ID of item                 |
|            | create_time       | Created Time               |
|            | modify_time       | Modified Time              |
|            | encrypted_title   | Encrypted Title Data       |
|            | encrypted_note    | Encrypted PIN or Memo Data |
|            | encrypted_content | Encrypted Other Contents   |
|            |                   |                            |


## V. 결론
패스워드 매니저 애플리케이션은 사용자의 아이디와 패스워드를 보관하고 있다. 범죄 수사 시 사용자의 행위 추적과 증거 수집에 활용할 수 있으므로 패스워드 매니저의 구조와 암호화 방식에 대한 연구가 필요하다.

본 연구에서는 패스워드 매니저 애플리케이션인 Proton Pass의 아티팩트를 복호화하고 이를 분석하였다. Proton Pass는 비밀 데이터를 32-byte 키로 암호화하여 db-passkey에 저장하며, 암호화에 사용한 키를 Android Keystore의 KEK로 암호화하여 pass.key 파일에 저장한다. db-passkey 파일 내에는 pass.key로 암호화된 데이터 내용, 수정 시각, 생성 시각 등이 남아있었다. 본 연구의 결과를 통해 'Proton Pass'의 디지털 증거 수집에 도움이 될 수 있을 것으로 기대한다.

## 참고문헌
[^1]: 조선일보, ""이러니 털리지" 세계서 가장 흔한 비밀번호는 '123456'...한국은?", https://www.chosun.com/economy/tech_it/2024/11/18/D3B326KQ5VDFJP4UVD7XFOCCGE/
[^2]: 머니투데이, ""기지국 관리 허술", KT 소액결제 침해, 내부자 연루설 '무게'", [https://www.mt.co.kr/tech/2025/09/12/2025091210402881831](https://www.mt.co.kr/tech/2025/09/12/2025091210402881831)
[^3]: Wikipedia, "Password manager", [https://en.wikipedia.org/wiki/Password_manager](https://en.wikipedia.org/wiki/Password_manager)
[^4]: FORENSICFOCUS, "The Potential Importance Of Information From Password Managers", [https://www.forensicfocus.com/articles/the-potential-importance-of-information-from-password-managers/](https://www.forensicfocus.com/articles/the-potential-importance-of-information-from-password-managers/
[^5]: Developers, "Android keystore system", [https://developer.android.com/privacy-and-security/keystore?hl=ko](https://developer.android.com/privacy-and-security/keystore?hl=ko
[^6]: 남기훈, 석병진, 공성현, 김역, and 이창훈, "패스워드 매니저 취약점을 이용한 사용자 계정 탈취 방법," 디지털포렌식연구, vol. 12, no. 1, pp. 9-18, 2018.
[^7]: 김수린 and 김형식, "패스워드 매니저의 보안성 분석," 정보보호학회지, vol. 28, no. 1, pp. 36-42, 2018.
[^8]: 박준성 and 이상진, "크롬 브라우저(Chrome)에서 구글 패스워드 매니저 데이터 획득에 대한 연구," 디지털포렌식연구, vol. 15, no. 2, pp. 12-23, 2021.
[^9]: 김한결, 이신영, and 박명서, "비밀번호 관리 어플리케이션의 주요 데이터 복호화 연구 및 보안성 평가," 정보보호학회논문지, vol. 34, no. 1, pp. 61-70, 2024.
[^10]: National Institute of Standards and Technology, "Data-Encryption-Key", [https://csrc.nist.gov/glossary/term/data_encryption_key](https://csrc.nist.gov/glossary/term/data_encryption_key
[^11]: National Institute of Standards and Technology, "Key-Encryption-Key", [https://csrc.nist.gov/glossary/term/key_encryption_key](https://csrc.nist.gov/glossary/term/key_encryption_key)