CNG로 AES를 동작시키는 대략적인 과정의 코드는 다음과 같다.

## Open Algorithm Provider
``` cpp
NTSTATUS BCryptOpenAlgorithmProvider(
	[out] BCRYPT_ALG_HANDLE *phAlgorithm, // Algorithm Handle의 주소
	[in]  LPCWSTR           pszAlgId,     // Cryptographic Algorithm 이름
	[in]  LPCWSTR           pszImplementation, // 보통 NULL로 설정
	[in]  ULONG             dwFlags       // FLAG로, 보통 0으로 설정
);

// Example code
NTSTATUS status;
BCRYPT_ALG_HANDLE algHandle = NULL;

status = BCryptOpenAlgorithmProvider(
	&algHandle,
	BCRYPT_AES_ALGORITHM,
	NULL,
	0);
if (!NT_SUCCESS(status)) { printf("Error\n"); return; } // 에러 코드 출력
```

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_NOT_FOUND`: `pszAlgId`, 즉 알고리즘의 ID를 찾을 수 없음
- `STATUS_NO_MEMORY`: 메모리 할당이 제대로 되지 않았음
- `STATUS_INVALID_PARAMETER`: 파라미터를 잘못 넣었음

![[Pasted image 20250810001131.png]]

## Create Key Object
```cpp
NTSTATUS BCryptGenerateSymmetricKey(
	[in,out]        BCRYPT_ALG_HANDLE hAlgorithm, // Algorithm Handle
	[out]           BCRYPT_KEY_HANDLE *phKey,     // Key Handle의 주소
	[out, optional] PUCHAR            pbKeyObject,// 보통 NULL로 설정
	[in]            ULONG             cbKeyObject,// 보통 0으로 설정
	[in]            PUCHAR            pbSecret,   // Key를 저장할 버퍼
	[in]            ULONG             cbSecret,   // Key를 저장할 버퍼의 크기
	[in]            ULONG             dwFlags     // 보통 0으로 설정
);

// Example code (위에서 이어짐)
BCRYPT_KEY_HANDLE KEY_HANDLE = NULL;
BYTE AES256Key[32] = { 0,  };
status = BCryptGenerateSymmetricKey(
	algHandle,
	&KEY_HANDLE,
	NULL,
	0,
	AES256Key,
	sizeof(AES256Key),
	0);
if (!NT_SUCCESS(status)) { printf("Error\n"); return; } // 에러 코드 출력
```

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_BUFFER_TOO_SMALL`: Key Object의 크기가 너무 작음
- `STATUS_INVALID_HANDLE`: `hAlgorithm`을 찾을 수 없음
- `STATUS_INVALID_PARAMETER`: 파라미터를 잘못 넣었음

## Set Operation Mode
```cpp
NTSTATUS BCryptSetProperty(
	[in,out] BCRYPT_HANDLE hObject,      // CNG Object
	[in]     LPCWSTR       pszProperty,  // 운용 모드 이름
	[in]     PUCHAR        pbInput,      // pszProperty의 주소
	[in]     ULONG         cbInput,      // pszProperty의 크기
	[in]     ULONG         dwFlags       // FLAGS
)

// Example code (위에서 이어짐)
status = BCryptSetProperty(
	KEY_HANDLE,
	BCRYPT_CHAINING_MODE,
	(PBYTE)BCRYPT_CHAIN_MODE_CBC,
	sizeof(BCRYPT_CHAIN_MODE_CBC),
	0);
if (!NT_SUCCESS(status)) { printf("Error\n"); return; } // 에러 코드 출력 
```

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_INVALID_HANDLE`: `hObject` 파라미터의 값을 찾을 수 없음
- `STATUS_INVALID_PARAMETER`: 파라미터를 잘못 넣었음
- `STATUS_NOT_SUPPORTED`: `pszProperty`의 값을 지원하지 않거나 읽기 전용으로만 접근할 수 있음

## Get IV length or Block length
```cpp
NTSTATUS BCryptGetProperty( 
	[in]   BCRYPT_HANDLE hObject,     // CNG Object
	[in]   LPCWSTR       pszProperty, // Property 이름
	[out]  PUCHAR        pbOutput,    // 버퍼의 주소
	[in]   ULONG         cbOutput,    // 버퍼의 크기
	[out]  ULONG         *pcbResult,  // 버퍼에 복사된 바이트의 수
	[in]   ULONG         dwFlags      // FLAGS
);

// Example code (위에서 이어짐)
status = BCryptGetProperty(
	KEY_HANDLE,
	BCRYPT_BLOCK_LENGTH,
	(PBYTE)&IVLength,
	sizeof(DWORD),
	&bufferSize,
	0);
if (!NT_SUCCESS(status)) { printf("Error\n"); return; } // 에러 코드 출력
BlockLength = IVLength;
```

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_INVALID_HANDLE`: `hObject` 파라미터의 값을 찾을 수 없음
- `STATUS_INVALID_PARAMETER`: 파라미터를 잘못 넣었음
- `STATUS_NOT_SUPPORTED`: `pszProperty`의 값을 지원하지 않거나 읽기 전용으로만 접근할 수 있음
- `STATUS_BUFFER_TOO_SMALL`: 버퍼 크기가 너무 작음

## Calculate Cipher/Plain Size
이 과정은 암호화 과정에서 연산될 암호문의 길이를 계산하거나, 복호화 과정에서 연산될 평문의 길이를 계산하는 과정으로, 밑에 있는 암/복호화 과정과 같은 함수인 `BCryptEncrypt/Decrypt`함수로 동작한다. 
(편의상 암호화 과정만 서술)
```cpp
NTSTATUS BCryptEncrypt(
  [in, out]           BCRYPT_KEY_HANDLE hKey,          // Key Handle
  [in]                PUCHAR            pbInput,       // Plaintext
  [in]                ULONG             cbInput,       // Plaintext의 크기
  [in, optional]      VOID              *pPaddingInfo, // 보통 NULL
  [in, out, optional] PUCHAR            pbIV,          // IV
  [in]                ULONG             cbIV,          // IV의 크기
  [out, optional]     PUCHAR            pbOutput,      // NULL로 설정
  [in]                ULONG             cbOutput,      // 0
  [out]               ULONG             *pcbResult,    // 크기를 받을 버퍼 주소
  [in]                ULONG             dwFlags        // Padding FLAGS
);

// Example code (위에서 이어짐)
status = BCryptEncrypt(
	KEY_HANDLE, 
	Plain, 
	sizeof(Plain), 
	NULL, 
	iv, 
	ivLength, 
	NULL, 
	0, 
	&CipherLength, 
	BCRYPT_BLOCK_PADDING);
if (!NT_SUCCESS(status)) { printf("Error\n"); return; } // 에러 출력 코드 
```

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_INVALID_HANDLE`: `hKey` 파라미터의 값을 찾을 수 없음
- `STATUS_INVALID_PARAMETER`: 파라미터를 잘못 넣었음
- `STATUS_NOT_SUPPORTED`: 해당 암호화 알고리즘을 지원하지 않음
- `STATUS_BUFFE임

## Close Algorithm Provider
```cpp
NTSTATUS BCryptCloseAlgorithmProvider(
	[in,out] BCRYPT_ALG_HANDLE hAlgorithm, // Algorithm handle
	[in]     ULONG             dwFlags     // FLAG
);

// Example code (위에서 이어짐)
status = BCryptCloseAlgorithmProvider(
	algHandle,
	0);
```

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_INVALID_HANDLE`: 해제할 Algorithm handle을 찾을 수 없음

---
## 더 간결하게 실행할 수 있는 방법
대칭키 암호화의 경우, 몇 가지 함수를 사용하지 않고도 간결하게 암호화나 복호화를 수행할 수 있다. 

`BCryptOpenAlgorithmProvider`함수와 `BCryptSetProperty`함수를 쓰지 않고 전역변수로 해당 사진처럼 미리 Algorithm handle을 선언해 간결하게 사용할 수 있다. 
![[Pasted image 20250810001359.png]]

또한, IV와 블록의 길이를 설정하는 함수인 `BCryptGetProperty`함수도 다음과 같이 전역 변수로 IV의 길이와 블록의 길이를 미리 설정하여 더 간결하게 사용할 수 있다.
![[Pasted image 20250810001526.png]]