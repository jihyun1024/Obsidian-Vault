## 요약
계정 기반 온라인 서비스의 확대로 비밀번호를 안전하고 효율적으로 저장하기 위해 패스워드 매니저의 사용이 증가하고 있다. 패스워드 매니저는 사용자의 비밀번호와 개인정보 등 민감 정보를 저장하므로 디지털 포렌식의 주요 분석 대상이다. 그러나 사용자 정보를 보호하기 위해 데이터를 암호화하는 경우가 많아, 이에 대한 복호화 연구가 필요하다. 본 논문은 Android 환경에서 패스워드 매니저 중 하나인 Proton Pass를 분석하였다. Proton Pass 애플리케이션은 데이터베이스 내 일부 컨텐츠를 암호화하고, 해당 암호키를 Android Keystore에 저장된 키로 다시 암호화하여 보관한다. 분석을 통해 Androind Keystore에 저장된 키를 획득하여 암호키를 복호화하였고, 복호화한 암호키로 암호화된 컨텐츠를 복호화하는 방안을 제시하였다. 이후, 데이터베이스 분석으로 생성 및 수정 시각, 비밀번호 등 주요 아티팩트를 정리하였다. 연구 결과를 통해 Proton Pass의 디지털 증거 수집에 기여할 수 있을 것으로 기대한다. 
## I. 서론
비밀번호는 사용자가 특정 서비스를 이용할 때 본인을 확인하는 기초적인 인증 수단이다. 다수의 사용자는 편의를 위해 동일한 비밀번호를 여러 시스템에 사용한다. 이러한 행위는 계정 정보가 유출될 경우, 다른 서비스에 무차별 대입되는 공격인 크리덴셜 스터핑 공격에 악용될 수 있다[^1]. 실제로 GS리테일은 크리덴셜 스터핑 공격으로 2024년 6월부터 2025년 2월까지 158만 건의 개인정보가 유출되었다[^2]. 이에 따라 전문가들은 비밀번호 재사용을 방지하기 위한 수단으로 패스워드 매니저의 사용을 권장하고 있다[^3].

패스워드 매니저는 비밀번호, 카드 정보 등의 사용자 정보를 안전하고 효율적으로 저장하는 데 사용된다. 패스워드 매니저에 저장된 데이터는 디지털 포렌식 수사에서 사용자의 로그인 시점 및 사용 서비스 등을 추적하는 데 중요한 증거로 사용될 수 있다[^4]. 하지만 일부 패스워드 매니저는 주요 데이터를 암호화하여 범죄 수사에 어려움을 준다. 따라서 원활한 디지털 포렌식 수사를 위해 암호화된 데이터를 복호화하는 연구가 선행되어야 한다.

본 논문에서는 패스워드 매니저 애플리케이션 Proton Pass v1.32.7을 Android 환경에서 분석하였다. 본 연구의 구성은 다음과 같다. 2장은 배경지식과 선행 연구를 정리한다. 3장은 데이터베이스 내 암호화된 컨텐츠를 복호화하는 방안을 제시한다. 4장은 주요 아티팩트를 분석하며, 마지막 5장은 결론으로 마무리한다.

## II. 배경 지식 및 관련 연구
### 2.1 배경 지식
Android Keystore는 Android 기기 또는 애플리케이션에서 사용하는 키를 보관 및 관리한다[^5]. 해당 시스템으로 보관된 키는 Android 운영체제 버전에 따라 저장되는 물리적 위치가 달라진다. 
Android 12 미만 버전의 경우, data/misc/keystore/User_0 디렉터리 내 개별 파일 형태로 저장된다. Android 12 이상 버전의 경우 data/misc/keystore 디렉터리 내 persistent.sqlite 데이터베이스에 저장된다.

### 2.2 관련 연구
패스워드 매니저 애플리케이션에 관한 연구는 지속적으로 이루어지고 있다. 남기훈 외 4인은 패스워드 매니저의 구조적 취약점을 바탕으로 사용자 계정 탈취 방안을 연구하였다[^6]. 박준성 외 1인은 크롬 브라우저의 구글 패스워드 매니저의 암복호화 과정을 분석하고 복구 도구를 제안하였다[^7]. 김한결 외 2인은 Android 환경에서의 패스워드 매니저 애플리케이션 4종의 암호화 알고리즘 및 취약점 보완 방안 연구를 진행하였다[^8].

## III. Proton Pass 데이터베이스 내 암호화된 컨텐츠 복호화 방안
Android 환경에서 Proton Pass 관련 데이터는 /data/data/proton.android.pass/databases 경로에 존재한다. Proton Pass에 기록한 사용자 정보는 db-passkey 데이터베이스에 존재하며, 일부 컨텐츠는 DEK(Data Encryption Key)로 암호화되어 저장된다. 컨텐츠 암호화에 사용된 DEK는 KEK(Key Encryption Key)로 암호화하여 pass.key 파일에 저장된다(그림 1).
![](file:///C:\Users\rkdwl\AppData\Local\Temp\Hnc\BinData\EMB00004b3812e0.png)
(그림 1) Structure of Proton Pass

본 장에서는 Proton Pass의 DEK를 복호화하고, 복호화한 DEK로 암호화된 컨텐츠를 복호화하는 방식을 설명한다.

### 3.1 DEK 복호화 방안
DEK는 AES256/GCM/NoPadding 알고리즘으로 복호화할 수 있다. 복호화에 사용되는 KEK는 Android Keystore에서 획득할 수 있다. IV, 암호화된 DEK 및 Tag는 pass.key 파일에 각각 12-byte, 32-byte, 그리고 16-byte 크기로 연접되어 저장된다. 상세 구조는 다음과 같다(그림 2).
![[Pasted image 20251106113013.png]]
(그림 2) Structure of pass.key

획득한 KEK, IV, 암호화된 DEK와 Tag를 사용하여 복호화할 수 있으며, 상세 복호화 과정은 (그림 3)과 같다.
![[Pasted image 20251106113056.png]]
(그림 3) DEK Decryption Process

### 3.2 암호화된 컨텐츠 복호화 방안
암호화된 컨텐츠는 AES256/GCM/NoPadding 알고리즘으로 복호화할 수 있다. 12-byte의 IV와 16-byte의 Tag는 각각 암호화된 컨텐츠의 앞과 뒤에 저장된다. 상세 구조는 다음과 같다(그림 4).
![[Pasted image 20251106113150.png]]
(그림 4) Structure of Encrypted Content

복호화한 DEK, IV, 암호화된 컨텐츠 및 Tag를 사용하여 복호화할 수 있으며, 상세 복호화 과정은 (그림 5)와 같다.
![[Pasted image 20251106113229.png]]
(그림 5) Content Decryption Process

## IV. Proton Pass 데이터베이스 아티팩트 분석
사용자 명, 이메일 등 사용자 계정 정보는 AccountEntity 테이블에 저장된다[표 1].

[표 1] Artifacts of User Information Data

| Table         | Column   | Content    |
| ------------- | -------- | ---------- |
| AccountEntity | username | User Name  |
|               | email    | User Email |

비밀번호, 생성 시각 등 사용자 기록과 관련된 정보는 ItemEntity 테이블에 저장된다[표 2].

[표 2] Artifacts of Password Data

| Table      | Column            | Content               |
| ---------- | ----------------- | --------------------- |
| ItemEntity | create_time       | Created Time          |
|            | modify_time       | Modified Time         |
|            | encrypted_title   | Encrypted Title       |
|            | encrypted_note    | Encrypted PIN or Memo |
|            | encrypted_content | Encrypted Contents    |


## V. 결론
본 논문에서는 패스워드 매니저 애플리케이션 Proton Pass의 암호화된 컨텐츠를 복호화하고, 아티팩트를 정리하였다. Proton Pass는 민감한 정보를 DEK로 암호화하였으며, DEK는 Android Keystore 내에 저장한 KEK로 암호화하였다. DEK 및 컨텐츠 암호화에 사용된 알고리즘은 AES256/GCM/NoPadding으로 동일하였으며, IV 및 Tag는 각각의 암호문과 연접하여 저장됨을 확인하였다. 분석한 결과를 토대로 암호화된 DEK와 컨텐츠를 복호화하는 데 성공하였다. 이후, 데이터베이스를 분석하여 비밀번호, 사용자 정보 등의 개인정보를 정리하였다. 본 연구의 결과를 바탕으로 디지털 포렌식 증거 수집에 도움이 될 것으로 기대된다.

## 참고문헌
[^1]: 보안뉴스, "크리덴셜 스터핑 공격, 재사용된 비밀번호로 글로벌 서비스 계정 무더기 털렸다", https://m.boannews.com/html/detail.html?idx=139335

[^2]: 보안뉴스, "GS리테일, 홈쇼핑만 158만건 개인정보 유출", https://www.boannews.com/media/view.asp?idx=136291

[^3]: The Guardian, "Internet users advised to change passwords after 16bn logins exposed", https://www.theguardian.com/technology/2025/jun/21/internet-users-advised-to-change-passwords-after-16bn-logins-exposed

[^4]: FORENSICFOCUS, "The Potential Importance Of Information From Password Managers", https://www.forensicfocus.com/articles/the-potential-importance-of-information-from-password-managers/

[^5]: Developers, "Android keystore system", https://developer.android.com/privacy-and-security/keystore?hl=ko

[^6]: 남기훈, 석병진, 공성현, 김역, 이창훈, "패스워드 매니저 취약점을 이용한 사용자 계정 탈취 방법," 디지털포렌식연구, vol. 12, no. 1, pp. 9-18, 2018.

[^7]: 박준성, 이상진, "크롬 브라우저(Chrome)에서 구글 패스워드 매니저 데이터 획득에 대한 연구," 디지털포렌식연구, vol. 15, no. 2, pp. 12-23, 2021.

[^8]: 김한결, 이신영, 박명서, "비밀번호 관리 어플리케이션의 주요 데이터 복호화 연구 및 보안성 평가," 정보보호학회논문지, vol. 34, no. 1, pp. 61-70, 2024.