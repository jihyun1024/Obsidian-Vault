## 요약
비밀번호 관리 애플리케이션은 사용자의 다양한 정보를 저장하고 있다. 이러한 점은 범죄 수사에서 중요한 증거가 될 수 있으므로 비밀번호 관리 애플리케이션에 대한 보안 구조와 암호화 방식에 대한 연구가 필요하다. 본 연구에서는 Proton Pass 애플리케이션을 분석하였다. 분석 결과 사용자의 정보는 암호화되어 저장되며 암호화에 사용된 키는 Android Keystore에서 관리하는 Master Key로 암호화되어 파일 형태로 저장된다. 본 연구에서는 복호화한 키를 이용해 암호화된 데이터를 복호화하고 이를 바탕으로 사용자 아티팩트 분석을 수행한다.
## I. 서론
비밀번호 관리자, 또는 패스워드 매니저(Password manager)는 컴퓨터나 모바일에 설치되어 사용자가 온라인 결제나 웹에서 사용하는 비밀번호를 안전하게 저장하고 효율적으로 관리할 수 있도록 하는 응용 프로그램이다[^1]. 현대 디지털 환경에서 사용자는 수십 개의 비밀번호를 관리해야 하며, 이 과정에서 비밀번호 재사용 혹은 '1q2w3e4r', '123456' 등 흔한 비밀번호 패턴 사용으로 만들어진 비밀번호 등 다양한 취약점으로 인한 공격으로 인해 사용자의 비밀 정보가 유출될 경우 다른 민감한 정보 또한 쉽게 노출될 위험이 있다[^2][^3]. 비밀번호 유출을 방지하기 위해 사용자의 편리함을 보장하는 동시에 안전하게 비밀번호를 관리할 수 있는 비밀번호 관리자가 필요하다.
비밀번호 관리자는 그 자체로 공격의 표적이 될 수 있다. 실제로 2022년 상용 비밀번호 관리 애플리케이션이 공격의 표적이 되어 500만 달러 상당의 암호화폐를 도난당한 사례가 존재하며, 이는 사용자의 다른 계정에 대한 위협으로 다가올 수 있다[^4]. 이와 같이 비밀번호 관리 애플리케이션은 사용자의 다양한 계정 정보를 저장하므로 범죄 수사에서 접근 권한, 행위 추적 등에 중요한 단서가 될 수 있어 비밀번호 관리자에 대한 보안 구조와 암호화 방식에 대한 연구가 중요하다. 
본 연구에서는 사용자의 비밀번호를 암호화하여 데이터베이스에 저장하는 비밀번호 관리 애플리케이션 Proton Pass의 암호화된 컨텐츠를 복호화하고 이어서 아티팩트를 분석한다. 본 연구의 구성은 다음과 같다. 2장에서는 Proton Pass 애플리케이션과 관련된 배경 지식과 선행 연구를 정리한다. 3장에서는 데이터베이스 내의 암호화된 비밀번호에 대한 복호화를 수행하고 Proton Pass의 주요 아티팩트를 설명한다. 마지막 4장은 결론으로 마무리한다. 분석 환경은 [표 1]과 같다. 

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
EncryptedSharedPreferences는 AndroidX Security 라이브러리의 구현체로, Key는 AES-256-SIV, Value는 AES-256-GCM으로 암호화하여 저장한다[^5]. 암호화에 사용되는 Master Key는 Android Keystore에서 관리되며 애플리케이션이 키의 평문을 직접 열람할 수 없도록 설계되어 있다. 또한 민감 데이터(예: vault 내용)는 디바이스 내에서만 복호화되며, 서버에는 암호화된 형태로만 전송·저장된다. 이러한 구조는 포렌식적 접근이나 단순 파일 추출만으로는 데이터 복호화가 어렵도록 보안성을 높인다. 

### 2.2 관련 연구
Android 환경이나 웹 브라우저 환경 등에서 패스워드 매니저에 대한 분석 연구는 지속적으로 이어지고 있다. 박준성·이상진은 크롬 브라우저에서 사용되는 계정 정보의 관리 방법과 암호화 방식을 확인하고 새로운 복호화 도구를 제안하였다[^6]. 김수린·김형식은 패스워드 매니저를 웹 기반과 로컬 기반으로 분류하고 각각에 대해 저장 및 관리 방법과 발생 가능한 취약점에 대해 연구하였다[^7]. 김한결 외 2인은 여러 Android 어플리케이션에서 비밀번호 암호화 알고리즘과 대상 어플리케이션의 취약점 및 보완 방안에 대해 연구하였다[^8].

## III. Proton Pass 복호화 및 아티팩트 분석
Android 환경에서 Proton Pass의 사용자 아티팩트는 /data/data/proton.android.pass 경로에 저장된다. 본 연구에서는 databases 디렉터리의 db-passkey 파일을 분석하였으며 해당 파일은 SQLite 포맷으로 구성되어 있다(그림 1).
![[Pasted image 20250924031224.png]]
(그림 1) Data Structure of Proton Pass

비밀번호와 이를 저장할 때 쓰인 제목은 db-passkey 파일 내 ItemEntity 테이블에 저장된다. 테이블에는 생성 및 수정 시각, 암호화된 컨텐츠 등이 저장되어 있다. 

### 3.1 데이터베이스 내 컨텐츠 복호화 방안
암호화된 비밀번호를 복호화하기 위한 복호화 키 생성에는 /data/data/proton.android.pass 경로에 60-byte로 저장되어 있는 pass.key 파일이 사용된다(그림 2). pass.key 파일이 존재하지 않을 경우, 32-byte 난수 배열을 생성한다.
![[Pasted image 20250924035829.png]]
(그림 2) pass.key file

IV는 pass.key의 상위 12-byte를 사용하며 Tag는 pass.key의 하위 16-byte를 사용한다. 암호화된 키인 pass.key의 나머지 byte는 AES256/GCM/NoPadding 알고리즘을 사용하여 복호화할 수 있다. 해당 과정을 정리하면 다음과 같다(그림 3).
![[Pasted image 20250924050040.png]]
(그림 3) Decryption Process of pass.key

AES 복호화에 사용되는 Masterkey는 /data/misc/keystore/user_X 디렉터리의 USRID_USRPKEY__me_proton_core_data_crypto_master_key_ 파일에 32-byte로 저장되어 있다. 

암호화된 컨텐츠는 AES256/GCM/NoPadding 알고리즘을 사용하여 복호화할 수 있다. 암호화된 컨텐츠를 Base64 디코딩하여 데이터로 정의하며, IV는 데이터의 상위 12-byte를 사용한다. 해당 과정을 정리하면 다음과 같다(그림 4).
![[Pasted image 20250924051949.png]]
(그림 4) Decryption Process of Encrypted Password

### 3.2 사용자 아티팩트 분석
사용자의 정보는 db-passkey 파일의 AccountEntity 테이블에 저장된다. 테이블에는 사용자 ID와 Email 등이 저장되어 있다. 주요 아티팩트를 정리한 내용은 [표 2]와 같다.

[표 2] Artifacts of User Data

| Table         | Column            | Content                                  |
| ------------- | ----------------- | ---------------------------------------- |
| AccountEntity | userId            | User ID                                  |
|               | username          | User Name                                |
|               | email             | User Email                               |

암호화된 데이터는 ItemEntity 테이블에 저장된다. 테이블에는 사용자 ID와 생성 시각, 제목, 비밀번호 등이 저장되어 있다. 주요 아티팩트를 정리한 내용은 [표 3]과 같다.

[표 3] Artifacts of Password Data

| Table      | Column            | Content                                  |
| ---------- | ----------------- | ---------------------------------------- |
| ItemEntity | id                | ID of item                               |
|            | user_id           | User ID                                  |
|            | address_id        | ID of User Address                       |
|            | create_time       | Created Time                             |
|            | modify_time       | Modified Time                            |
|            | encrypted_title   | Encrypted Title Data                     |
|            | encrypted_note    | Encrypted PIN or Memo Data               |
|            | encrypted_content | Encrypted Other Contents                 |
|            | encrypted_key     | Encrypted Data Encryption Key (60-bytes) |

## IV. 결론
패스워드 매니저 기술은 사용자의 여러 가지 정보를 보관하고 있어 범죄 수사 시 사용자의 행위 추적과 증거 수집에 활용할 수 있으므로 패스워드 매니저의 구조와 암호화 방식에 대한 연구가 필요하다. 
본 연구에서는 패스워드 매니저 애플리케이션인 Proton Pass의 아티팩트를 복호화하고 이를 분석하였다. Proton Pass는 비밀 데이터를 32-byte 키로 암호화하여 db-passkey에 저장하며, 암호화에 사용한 키를 Android Keystore의 Master key로 암호화하여 pass.key 파일에 저장한다. db-passkey 파일 내에는 복호화에 성공한 데이터 내용, 수정 시각, 만들어진 시각 등이 남아있었다. 본 연구의 결과를 통해 'Proton Pass'의 디지털 증거 수집에 도움이 될 수 있을 것으로 기대한다.

## 참고문헌
[^1]: Wikipedia, "Password manager", https://en.wikipedia.org/wiki/Password_manager
[^2]: 조선일보, ""이러니 털리지" 세계서 가장 흔한 비밀번호는 '123456'...한국은?", https://www.chosun.com/economy/tech_it/2024/11/18/D3B326KQ5VDFJP4UVD7XFOCCGE/
[^3]: 박소희, Yazmyradov Serdar, and 이훈재, "국내에서 많이 사용되는 비밀번호 유형과 보안 동향," 한국컴퓨터정보학회 학술발표논문집 , vol. 32, no. 2, pp. 253-255, 2024.
[^4]: BLEEPINGCOMPUTER, "US seizes $23 million in crypto linked to LastPass breaches", https://www.bleepingcomputer.com/news/security/us-seizes-23-million-in-crypto-stolen-via-password-manager-breach/
[^5]: Developers, "EncryptedSharedPreferences", https://developer.android.com/reference/androidx/security/crypto/EncryptedSharedPreferences
[^6]: 박준성 and 이상진, "크롬 브라우저(Chrome)에서 구글 패스워드 매니저 데이터 획득에 대한 연구," 디지털포렌식연구, vol. 15, no. 2, pp. 12-23, 2021.
[^7]: 김수린 and 김형식, "패스워드 매니저의 보안성 분석," 정보보호학회지, vol. 28, no. 1, pp. 36-42, 2018.
[^8]: 김한결, 이신영, and 박명서, "비밀번호 관리 어플리케이션의 주요 데이터 복호화 연구 및 보안성 평가," 정보보호학회논문지, vol. 34, no. 1, pp. 61-70, 2024.