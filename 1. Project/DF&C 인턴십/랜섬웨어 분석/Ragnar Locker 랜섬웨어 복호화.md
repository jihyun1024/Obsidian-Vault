### 랜섬웨어 복호화 가능성 파악
- ARX 구조: 덧셈, 비트 시프트, XOR 연산이 주된 연산으로 작용하는 암호화 알고리즘
	- 스트림 암호 등 경량 암호 알고리즘에 주로 사용됨
	- 덧셈: 비선형성을 위해 사용
	- 비트 시프트: 확산 효과를 위해 사용
	- XOR: 키나 데이터에 대해 사용

- XOR 연산의 성질
	- A ^ B = C일 때, A를 얻기 위해서는 C ^ B = A ^ B ^ B = A가 됨
	- 해당 원리를 이용해 **파일 암호화에 사용된 키가 재사용되었다는 가정 하에** XOR로도 복호화가 가능하다. 

- Salsa20의 Initial state에서 생성된 암호키는 모든 파일의 암호화에서 재사용된다. 
	- 즉, `원본 파일 ^ 암호화된 파일 = 키` 가 되며
	- 다른 파일들은 해당 키를 가지고 전부 복호화가 가능하다. 

### 랜섬웨어에 감염된 파일의 구조
XOR 연산의 원리를 이용하는 것이기 때문에, 랜섬웨어의 복호화를 위해서는 랜섬웨어에 감염된 파일이 필요하다. 
그런데 감염된 파일을 직접 빼 올 수는 없고, 그렇다고 직접 HxD에 하나하나 칠 수는 없으니, 간단한 텍스트 파일을 최상위 디렉터리의 가장 빠르게 암호화 되는 곳에 집어넣어 암호화를 시키고, 암호화된 값을 호스트 컴퓨터의 HxD에서 작업해서 Salsa20에서 사용했던 키스트림을 알아내는 방법이 가장 좋은 방법이다.

샘플 파일로 간단한 텍스트 파일과 인터넷에서 누구나 다운받을 수 있는 JPG 파일을 준비했다. 
랜섬웨어를 실행시킨 결과는 다음과 같았다. 
- txt 파일
	- 평문: 0x0C bytes
	- 암호문: 0x217 bytes
		- 평문에 비해 암호문의 길이가 엄청나게 길어짐 (0x200 bytes 추가됨)
		- 맨 끝에 일종의 시그니처인 `_RAGNAR_.` 존재 (0x09 bytes)
- jpg 파일
	- 평문: 0xD6B22 bytes
	- 암호문: 0xD6D2B bytes = 0xD6B22 + 0x09 + 0x200

스트림 암호는 XOR을 기반으로 동작, 생성된 키 스트림과 평문을 비트 단위로 XOR하기 때문에, 평문과 암호문의 바이트 길이는 동일하다. 
따라서, txt 파일에서 붙은 0x200 bytes는 평문과 상관 없는 바이트 배열이다. (시그니처 제외)

### 랜섬웨어 복호화
위에서 서술했던 대로, .txt 파일을 똑같이 하나 더 만들어서 암호문과 XOR해서 짧지만 키스트림의 일부를 알아낼 수 있다. 
![[Pasted image 20250827233233.png]]
이렇게 해서 나온 결과인 키스트림의 일부는 `66c3666e4b3f9f29971294aa`이다. 

따라서, .jpg 파일 또한 이와 같은 방식으로 복호화 할 수 있는데, 0x200 + 0x09 bytes를 제외한 암호화된 파일과 원본 파일을 XOR해 키스트림을 얻을 수 있고, 이렇게 얻은 키스트림을 이용해 다시 암호화된 파일과 XOR해서 원본 파일을 얻을 수 있다. 

이때 키스트림 길이는 평문의 길이에 따라 달라지기 때문에, 확보한 키스트림의 길이만큼만 복호화가 가능하다. 따라서 더 큰 파일을 복호화 하려면 큰 사이즈의 암호화된 파일과 그에 해당하는 원본 파일을 가지고 서로 XOR 해서 큰 사이즈의 키스트림을 찾아야 한다.

예시로 들었던 이미지 파일에서 키스트림의 일부를 추출해서 텍스트 파일을 복구한 Python 코드와 실행 결과는 다음 사진과 같다. 
```python
# ciphertext와 plaintext 쌍
# keystream을 계산하고, 이를 이용해 새로운 plaintext를 복원하는 코드
# ciphertext와 plaintext는 크면 클수록 키스트림 또한 커지기 때문에 더 큰 파일을 복구할 수 있음

ciphertext = b'\xFF\xD8\xFF\xE0\x00\x10\x4A\x46\x49\x46\x00\x01\x02\x01\x00\x60\x00\x60\x00\x00\xFF\xEE\x00\x0E\x41\x64\x6F\x62\x65\x00\x64\x00\x00\x00\x00\x01\xFF\xE1\x13\x5D\x45\x78\x69\x66\x00\x00\x4D\x4D\x00\x2A\x00\x00\x00\x08\x00\x07\x01\x32\x00\x02\x00\x00\x00\x14'

plaintext =  b'\x99\x1B\x99\x8E\x4B\x2F\xD5\x6F\xDE\x54\x94\xAB\xEB\xBB\x33\x2D\x11\x5A\xB6\x30\x65\x72\x9C\xC6\xBA\xCD\x1C\x02\xD7\x94\xB2\xB8\xFF\x9E\xE6\x03\xE9\xAA\x16\xAC\x2A\x6D\x5E\x5F\xDD\x29\xDC\xCE\x33\x90\xE9\x77\x68\xA8\x26\xE0\x42\x6A\x94\xE7\xED\x95\xEB\xC5'

  
# keystream 계산(XOR 연산)
keystream = bytes([c ^ p for c, p in zip(ciphertext, plaintext)])
print("Keystream:", keystream)
print("Keystream (hex):", keystream.hex())


# keystream의 일부와 ciphertext_1 쌍 (새로운 plaintext를 복원하기 위해 사용)
keystream_1 = b'\x66\xC3\x66\x6E\x4B\x3F\x9F\x29\x97\x12\x94\xAA'
ciphertext_1 = b'\x2E\xA6\x0A\x02\x24\x1F\xE8\x46\xE5\x7E\xF0\x8B'


# 새로운 plaintext 계산(XOR 연산)
result = bytes([c ^ p for c, p in zip(keystream_1, ciphertext_1)])
  

# 결과 출력
print("Result:", result)
print("Result (hex):", result.hex())
```
![[Pasted image 20250828030958.png]]
이 때, 키스트림을 복구할 `ciphertext` 와 `plaintext` 변수의 값을 더 크게 잡을수록 같은 키를 재사용하는 Ragnar Locker 랜섬웨어의 특성 상 복구할 수 있는 파일의 크기도 더욱 커진다. 
