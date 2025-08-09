- [[#Open Algorithm Provider|Open Algorithm Provider]]
- [[#Generate Random Number|Generate Random Number]]
- [[#Close Algorithm Provider|Close Algorithm Provider]]


CNG로 RNG(Random Number Generator)를 동작시키는 대략적인 과정의 코드는 다음과 같다. 

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
	BCRYPT_RNG_ALGORITHM,
	NULL,
	0);
if (!NT_SUCCESS(status)) return; // 에러 코드 출력
```

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_NOT_FOUND`: `pszAlgId`, 즉 알고리즘의 ID를 찾을 수 없음
- `STATUS_NO_MEMORY`: 메모리 할당이 제대로 되지 않았음
- `STATUS_INVALID_PARAMETER`: 파라미터를 잘못 넣었음

## Generate Random Number
```cpp
NTSTATUS BCryptGenRandom(
	[in,out]  BCRYPT_ALG_HANDLE hAlgorithm, // Algorithm Handle
	[in,out]  PUCHAR            pbBuffer,   // 난수를 저장할 버퍼
	[in]      ULONG             cbBuffer,   // 난수의 크기
	[in]      ULONG             dwFlags     // FLAG
);

// EXample code (위에서 이어짐)
BYTE random[16] = { 0,  };
status = BCryptGenRandom(
	algHandle,
	random,
	16,
	BCRYPT_RNG_USE_ENTROPY_IN_BUFFER);
if (!NT_SUCCESS(status)) return; // 에러 코드 출력 
```

플래그
![[Pasted image 20250809202945.png]]

반환값
- `STATUS_SUCCESS`: 함수가 제대로 실행됨
- `STATUS_INVALID_HANDLE`: Algorithm handle을 찾을 수 없음
- `STATUS_INVALID_PARAMETER`: 파라미터를 잘못 넣었음

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