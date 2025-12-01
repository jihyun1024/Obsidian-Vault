[[Proton Pass 기능 정리 및 아티팩트 분석]]에서 `pass.key`파일을 확인했는데, 이 파일은 SQLCipher로도 열리지 않아 암복호화 알고리즘에서 키로 사용하는 것으로 추측했었다. 
따라서, 우리는 해당 파일에 대해 먼저 APK에서 Jadx를 사용해 분석해 보자. 

![[Pasted image 20250728205057.png]]
Jadx에서 `pass.key`를 검색하면 해당 결과가 나오며, 이 메서드를 천천히 분석해 보자. 

![[Pasted image 20250728223759.png]]
이 부분은 개발자가 정의한 클래스를 사용해 Java에서 읽기와 쓰기 작업을 원활하게 처리할 수 있도록 하는 [[읽기,쓰기 락]] 작업을 수행한다. 이후 `byte[] bArr = encryptionContextProviderImpl.storedKey`에서 이미 저장된 키가 있는지 확인해서 키가 없다면 `readlock`을 해제하고 그 다음 `try`문을 처리한다. 

![[Pasted image 20250728233933.png]]
그 다음의 `try`문을 살펴보면 방금 전과 비슷하게 동작하나, `pass.key`파일을 열고 파일이 존재하면 해당 파일을 복호화하는 코드가, 파일이 존재하지 않으면 32바이트 키를 새로 생성하고 해당 키를 암호화하는 코드가 있는 것을 확인할 수 있다. 
우리는 암호화가 아닌 복호화에 관심이 있으니 복호화하는 코드만 찾아보자.

![[Pasted image 20250729000049.png]]
그러나, `decrypt`를 클릭해서 들어가 보면 함수의 인터페이스만 구현되어 있고, 정작 우리가 궁금한 메서드의 원본은 보이지 않는다. 이럴 때는 패키지에서 `KeyStoreCrypto`를 구현한 로직을 찾아야 한다. 
`me.proton.core.crypto.android.keystore`경로에 들어가면 `AndroidKeyStoreCrypto`라는 클래스가 보이며, 해당 코드에 `implements keyStoreCrypto`라고 해서 이를 구현한 로직이 있다. 
여기에 `decrypt`함수가 정의되어 있다. 

![[Pasted image 20250729002905.png]]
이 사진이 `decrypt`함수의 원본이다. 
이 함수에서 실제로 복호화를 수행하는 것은 `decryptOrRetry`함수이며, 이를 찾아가 보자, 
![[Pasted image 20250729031144.png]]
이 코드가 `decryptOrRetry`함수이고, 그 안에는 `ApiProvider$$ExternalSyntheticLambda0`함수가 들어있다. 그 함수로 들어가 보면 
![[Pasted image 20250729035018.png]]
이런 코드가 보이며, 이 코드는 네 번째 인자로 숫자 `i`를 받아 해당 숫자에 따라 `$r8$classId`가 결정되고, 결정된 `$r8$classId`에 따라 `switch`문이 동작해 암호화를 할지, 복호화를 할지 결정하는 코드이다. 
이 때, 이 전의 사진을 보면 `0x2`를 4번째 인자로 넣는 것을 확인할 수 있어 `decryptSync$crypto_android_release`가 동작하는 것을 볼 수 있으며, 그 함수에서 `pass.key`에 대한 실제 복호화가 진행된다. 

![[Pasted image 20250729041630.png]]
이 메서드가 실제 복호화가 진행되는 코드이며, 아까 전, 이 코드가 정의된 클래스인 `AndroidKeyStoreCrypto` 클래스가 선언되는 부분에서 확인했듯이, 다음과 같은 사항이 적용된다. 
1. 암호 알고리즘: AES256/GCM/NoPadding
2. MasterKey: [[AndroidKeyStore]]에 저장
3. IV 길이: 12 byte
4. Tag 길이: 128 bit (16 byte)
이는 이 메서드 안의 코드인 `GCMParameterSpec`에서도 찾아볼 수 있는데, 이 클래스는 GCM 알고리즘을 사용할 때 꼭 필요한 IV와 TAG의 길이를 지정하는 역할을 한다. 
이 메서드의 역할을 살펴보면 처음 두 줄에서는 `value`와 `key`가 `null`이 아닌지 검사하며, 이후 AES/GCM 용 Cipher 인스턴스를 생성하는 `cipherFactory`를 호출해 `Cipher`타입의 인스턴스를 생성하며, `byte[] copyOf = Arrays.copyOf(value.getArray(), 0xc)`를 사용해 IV를 12 byte만큼 암호화된 전체 바이트 배열에서 분리한다. 
따라서 암호화된 바이트 배열에서 12 byte 이후 나머지는 암호문과 GCM의 인증에서 사용되는 인증 태그가 되며, `cipher.init`으로 복호화 모드(`0x2`)로 `Cipher`객체를 초기화한다. 
이후 `cipher.doFinal`로 암호문과 태그에 대해 복호화를 수행하며, 인증 태그에 대한 검증 또한 같이 수행하여 복호화된 평문 바이트 배열을 `PlainByteArray`로 감싸서 반환한다. 

이제 복호화 과정은 어느 정도 살펴보았으니 [[AndroidKeyStore]]에서 MasterKey를 추출해 보자. 
내가 실행했던 환경에서 MasterKey는 data/misc/keystore/user_0 디렉터리의 10066_USRPKEY__me_proton_core_data_crypto_master_key_ 파일에 있었다. 
해당 파일을 열어보면 다음과 같이 보여지며, 그 중 MasterKey는 특정한 위치에 존재한다. 
![[Pasted image 20250729051317.png]]
따라서 해당 위치의 32byte가 MasterKey라는 것을 알 수 있다. 

이제 어느 정도 가닥을 잡았으니, 우리가 복호화하고자 하는 값인 `db-passkey`파일의 itemEntity 테이블에서 찾을 수 있는 암호화된 값인 encrypted_title과 encrypted_note값을 각각 전부 복호화 할 수 있다. 
![[Pasted image 20250729053901.png]]
예시로 해당 테이블에서 4번째 행의 값을 복호화 해 보자. 복호화의 순서는 다음과 같다. 
1.pass.key 복호화
	1. 복호화 알고리즘: AES256/GCM/NoPadding
	2. iv = pass.key[:12]
	3. ciphertext = pass.key[12:]
	4. Key: AndroidKeyStore에 저장되어 있는 MasterKey

2.db-passkey 파일의 암호화된 값 복호화
	1. 복호화 알고리즘: AES256/GCM/NoPadding
	2. 암호화된 데이터베이스의 값을 Base64로 디코딩
	3. Key: pass.key의 복호화된 ciphertext 값[:32]
	4. iv: 암호화된 값[:12]
	5. Ciphertext: 암호화된 값[12:-16]
	6. Tag: 암호화된 값[-16:]

이를 적용해 Python 코드를 제작하면 다음과 같은 코드를 짤 수 있다. 
```python
from Crypto.Cipher import AES
import base64


# pass.key 복호화
with open('pass.key', 'rb') as f:
    data = f.read()
  
iv = data[:12]
ciphertext = data[12:-16]
  
# KeyStore Blob에서 추출한 masterkey
masterkey = bytes.fromhex("9CE131042C30EB4A46D70E746D6C028018CEA0809A55DC4885FC421DA0AD4B53")
cipher = AES.new(masterkey, AES.MODE_GCM, nonce=iv)
dec_passkey = cipher.decrypt(ciphertext)
  
# 암호화된 아티팩트 복호화
# encrypted_note의 경우, PIN, Memo 데이터만 해당
encrypted_title = base64.b64decode('fugixgbir7NC2abkg14jDwisu4Pk/lWqNZncbJ01obgC0qtVCdjK')

encrypted_note = base64.b64decode('xUCGBd3QeEgocIpj0Hq601RsbpCE0Na0FARBvOpEOtlmHTtho8B7Sbj36nefySv5SwIajWsHzfxhOOAavATAd0ZjXpYcWw==')

# 이 부분만 복호화해도 웬만한 데이터는 다 나옴 (휴지통에 있는 데이터도 복호화 가능)

encrypted_content = bytes.fromhex("""7f439dc204723a63d61928de28fac1a5
c9ee7ca9c90da806f67a34f591ee92dc
58747b84ca04ce51ffd5d89d619c0668
804441dc4192f605d5141be48f4b184e
8b4645a5f69e3f6857f2e269c17cf815
666b32ee73a4b47ab79d93b6befea053
4408a982a98c5c9e1c64eb844c10880f
8c6547fdb72fe02922079b912fb765e3
f4""")
  
# iv, ciphertext, tag 분리
iv_title = encrypted_title[:12]
ciphertext_title = encrypted_title[12:-16]
tag_title = encrypted_title[-16:]
  
iv_note = encrypted_note[:12]
ciphertext_note = encrypted_note[12:-16]
tag_note = encrypted_note[-16:]

iv_content = encrypted_content[:12]
ciphertext_content = encrypted_content[12:-16]
tag_content = encrypted_content[-16:]

# 복호화
cipher_title = AES.new(dec_passkey, AES.MODE_GCM, nonce=iv_title)
dec_title = cipher_title.decrypt(ciphertext_title)

cipher_note = AES.new(dec_passkey, AES.MODE_GCM, nonce=iv_note)
dec_note = cipher_note.decrypt(ciphertext_note)

cipher_content = AES.new(dec_passkey, AES.MODE_GCM, nonce=iv_content)
dec_content = cipher_content.decrypt(ciphertext_content)


print("Decrypted Title:", dec_title.decode('utf-8'))
print("Decrypted Note:", dec_note.decode('utf-8'))
print("Decrypted Content:", dec_content.decode('utf-8'))
```
이 코드를 실행하면 ![[Pasted image 20250729054445.png]]
이와 같은 결과를 볼 수 있으며, 따라서 이 코드가 잘 동작한다는 것을 확인했다.