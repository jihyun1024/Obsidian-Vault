Android OS에서 제공하는 보안 키 저장소로, 애플리케이션이 암호화 키를 안전하게 저장하고 사용하는 방법을 제공하며, 키를 하드웨어 보안 모듈 또는 TEE(Trusted Execution Environment)에 저장할 수 있어 키 유출 위험을 크게 줄여준다. 

| 항목                | 설명                                       |
| ----------------- | ---------------------------------------- |
| 사용 목적             | 키를 안전하게 저장하고, 복호화 등 암호화 연산에 사용           |
| 저장 위치             | 소프트웨어(KeyStore) 또는 하드웨어(TEE / StrongBox) |
| 추출 가능 여부          | ❌ 키는 AndroidKeyStore에서 외부로 꺼낼 수 없음       |
| 연동 가능한 보안 기능      | 지문/얼굴 인식, PIN, 패턴 등                      |
| Android 버전별 기능 차이 | Android 6.0 이상부터 AES 지원, 9.0 이상 HMAC 지원  |
### 주요 키 생성 방식
#### 대칭키 (AES)
```java
KeyGenerator keyGenerator = KeyGenerator.getInstance(
    KeyProperties.KEY_ALGORITHM_AES, "AndroidKeyStore");

keyGenerator.init(new KeyGenParameterSpec.Builder("myKeyAlias",
    KeyProperties.PURPOSE_ENCRYPT | KeyProperties.PURPOSE_DECRYPT)
    .setBlockModes(KeyProperties.BLOCK_MODE_GCM)
    .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE)
    .setUserAuthenticationRequired(true)  // 생체 인증 필요 시
    .build());

SecretKey key = keyGenerator.generateKey();
```
단, AES 키는 AndroidKeyStore 안에서만 생성 가능하며 외부에서의 import는 불가능하다. 

#### 비대칭키 (RSA)
```java
KeyPairGenerator generator = KeyPairGenerator.getInstance(
    KeyProperties.KEY_ALGORITHM_RSA, "AndroidKeyStore");

generator.initialize(new KeyGenParameterSpec.Builder("myKeyAlias",
    KeyProperties.PURPOSE_ENCRYPT | KeyProperties.PURPOSE_DECRYPT)
    .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_RSA_OAEP)
    .build());

KeyPair keyPair = generator.generateKeyPair();
```
RSA 키는 공개키를 외부로 내보낼 수 있지만, 개인키는 KeyStore에만 존재한다. 

### 인증 정책 적용
- 생체 인증이나 사용자 인증을 요구할 수 있음
- 키를 사용할 때마다 화면 잠금 해제 또는 지문 인증 요구 가능
```java
.setUserAuthenticationRequired(true)
.setUserAuthenticationValidityDurationSeconds(30)
```

| 설정                                              | 설명                          |
| ----------------------------------------------- | --------------------------- |
| setUserAuthenticationRequired(true)             | 사용자 인증 필수                   |
| setUserAuthenticationValidityDurationSeconds(n) | 인증 후 n초 동안 재인증 없이 사용 가능     |
| setUserAuthenticationParameters(...)            | 생체 인증 요구 가능 (Android 11 이상) |
### 암호화 / 복호화 예시 (AES/GCM)
```java
// 암호화
Cipher cipher = Cipher.getInstance("AES/GCM/NoPadding");
cipher.init(Cipher.ENCRYPT_MODE, secretKey);
byte[] iv = cipher.getIV();
byte[] ciphertext = cipher.doFinal(plaintext);

// 복호화
GCMParameterSpec spec = new GCMParameterSpec(128, iv);
cipher.init(Cipher.DECRYPT_MODE, secretKey, spec);
byte[] decrypted = cipher.doFinal(ciphertext);
```

### 저장된 키 확인
```java
KeyStore ks = KeyStore.getInstance("AndroidKeyStore");
ks.load(null);
Enumeration<String> aliases = ks.aliases();
while (aliases.hasMoreElements()) {
    Log.d("KeyAlias", aliases.nextElement());
}
```

### 키 저장 구조
키는 AndroidKeyStore 내부에만 존재하며, 애플리케이션은 참조만 가능하다. 
```
[AndroidKeyStore]
 └─ "myKeyAlias"
     └─ SecretKeySpec (AES 256)
         └─ GCM block mode + 128bit authentication tag
```

### 저장 디렉터리
* Android 12 미만 버전: data/misc/keystore/user_X 디렉터리에 저장
* Android 12 이상 버전: data/misc/keystore 디렉터리 내 `persistent.sqlite`데이터베이스에 저장