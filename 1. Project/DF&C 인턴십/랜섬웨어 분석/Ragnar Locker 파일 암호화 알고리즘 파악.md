관련 문서: [[x64dbg로 Ragnar Locker 코드 흐름 파악 (동적 분석)]]

# 개요 및 함수 확인
위의 문서에서 이어지는 내용으로, `ragnar_locker.591950`함수는 파일을 하나하나 돌면서 암호화할 대상 파일을 탐색하는 함수로 추측되며, 그 안에서 호출되는 `ragnar_locker.591490`함수와 해당 함수가 호출하는 **피호출자(함수가 호출하는 또 다른 함수)가 해당 파일을 암호화하는 함수로 추측된다.**

`ragnar_locker.591490`함수가 호출하는 피호출자는 순서대로 다음과 같다. 
1. [CreateFileW](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-createfilew) 함수
2. [GetFileSizeEx](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-getfilesizeex) 함수
3. `ragnar_locker.5922B0` 함수 (분석 결과 뭐 없음)
4. `ragnar_locker.597480` 함수 (분석 결과 뭐 없음)
5. [SetFilePointerEx](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-setfilepointerexhttps://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-setfilepointerex) 함수
6. [GetProcessHeap](https://learn.microsoft.com/ko-kr/windows/win32/api/heapapi/nf-heapapi-getprocessheap) 함수
7. [HeapAlloc](https://learn.microsoft.com/ko-kr/windows/win32/api/heapapi/nf-heapapi-heapalloc) 함수
8. [ReadFile](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-readfile) 함수
9. `ragnar_locker.5926B0` 함수
10. [VirtualAlloc](https://learn.microsoft.com/ko-kr/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc) 함수
11. `ragnar_locker.592310` 함수
12. [LockFile](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-lockfile), [WriteFile](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-writefile), [UnlockFile](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-unlockfile) 함수
13. [VirtualFree](https://learn.microsoft.com/ko-kr/windows/win32/api/memoryapi/nf-memoryapi-virtualfree) 함수
14. [CloseHandle](https://learn.microsoft.com/ko-kr/windows/win32/api/handleapi/nf-handleapi-closehandle) 함수

이 함수들이 함께 사용된 코드를 보면, 파일을 메모리에 로드하고 처리한 뒤 다시 저장하는 일련의 작업을 수행하는 것으로 추측할 수 있다. 
이러한 코드는 `ragnar_locker.59_`접두어가 붙은 함수를 제외하면 다음과 같이 사용될 수 있다. 

1. **파일 열기 및 크기 확인**
	- `CreateFileW`로 파일을 열고, 
	- `GetFileSizeEx`로 파일 크기를 확인한 뒤, 
	- `SetFilePointerEx`로 읽기 위치를 설정
2. **메모리 할당 및 파일 읽기**
	- `GetProcessHeap`과 `HeapAlloc`으로 힙 메모리를 할당하거나
	- `VirtualAlloc`으로 가상 메모리 할당 후, 
	- `ReadFile`로 파일의 내용을 메모리에 로드
3. **파일 처리 및 쓰기**
	- `LockFile`로 파일의 특정 영역을 잠근 후
	- `WriteFile`로 데이터를 파일에 씀.
	- `UnlockFile`로 잠금 해제
4. **자원 해제**
	- `VirtualFree` 또는 `HeapFree`로 메모리 해제.
	- `CloseHandle`로 파일의 핸들 닫기

따라서, 이런 함수 조합은 파일을 메모리에 로드하고, 처리해 다시 저장하는 [[CNG (Cryptography Next Generation)]]와 비슷하게 파일의 암호화나 복호화에 사용될 수 있다. 
그러면, 더 자세하게 분석해 보자. 

# Ghidra를 사용한 분석에 대한 분석 방법
그 전에, [[Ghidra]]를 사용해 해당 함수에서 암호화(Salsa20, RC4 등)를 수행하는 부분이 있는지 분석해 볼 것이다.
Salsa20의 경우, constant word로 `expand 32-byte k`라는 문자를 Initial State 안에 사용하며, 해당 문자열은 `expa`, `nd 3`, `2-by`, `te k`의 알파벳 4개로 구성된 4개의 단어로 나눠서 사용된다. 
따라서 Ghidra에서는 
1. 문자열 검색: Ghidra의 Strings 창에서 `expand 32-byte k`를 직접 검색
2. Hex 값 검색: 워드를 각각 Hex로 검색
3. 디컴파일된 코드에서 확인: 디컴파일된 C 언어 코드에서 `static const unsigned char sigma[16] = "expand 32-byte k"`등의 형태로 추출
등의 방식으로 탐색해 볼 수 있다.[^1]

# x64dbg를 사용한 분석
다시 x64dbg로 넘어와서, Salsa20 알고리즘을 사용하는지 확인하는 방법은 앞에서 서술한 대로 Initial State의 4개의 단어로 이루어진 문자열 `expand 32-byte k` 문자열을 확인하거나, 
아래 사진의 Salsa20 알고리즘 구조도[^2]를 참고해
해당 구조처럼 동작하는 함수나 코드가 있는지 확인하는 방법으로 판단할 수 있다. 
>[!example]- Salsa20 알고리즘 구조도
![[Pasted image 20250823230030.png]]
- Key Size: 128 or 256 bits (16 or 32 bytes)
- State Size: 512 bits (64 bytes)
	- 8 words of key
	- 2 words of stream position
	- 2 words of nonce
	- 4 words of fixed words (`expand 32-byte k`)
		- 0th index: `expa`(ASCII), `0x61707865`(Hex)
		- 5th index: `nd 3`(ASCII), `0x3320646e`(Hex)
		- 10th index: `2-by`(ASCII), `0x79622d32`(Hex)
		- 15th index: `te k`(ASCII), `0x6b206574`(Hex)
		- Littie-Endian 방식으로 저장되기 때문에, 해당 바이트 시퀀스를 역순으로 해석 가능
- Structure: [[ARX 구조|ARX]]
	- 사용 연산: **A**dd, **R**otation, **X**OR
	- A, R, X가 주된 연산이고, 20 rounds로 이루어진 코드가 있을 경우, 해당 코드가 Salsa20
- Round: 20 rounds
- Designer: Daniel J. Bernstein

이제 분석 방법도 알고, 어떤 걸 찾아야 할지도 알았으니 x64dbg로 분석해 보자. 

---
`ragnar_locker.591490`함수로 들어가기 위해서는 F7, F8, F9을 번갈아 누르면서 건너뛸 것은 건너뛰고 분석할 부분만 분석해야 한다. 
![[Pasted image 20250826131032.png]]
이 부분에서 중단점이 `je ragnar_locker.5918A4` 분기 밑에 있는데, `je` 명령어는 ZF(Zero Flag)가 1일 때 점프를 수행한다. 이 분기 바로 밑에 있는 중단점 부분을 분석하기 위해서 ZF 부분을 두 번 눌러서 ZF = 0으로 만들어 강제로 들어갈 수 있다. 
또한, 이 문서 맨 위에서 `ReadFile` 함수와 `VirtualAlloc` 함수 뒤에 나오는 (파일을 읽고, 가상 메모리를 할당해 파일 암호화를 위한 프로세스를 할당) `ragnar_locker.592310` 함수에 집중해서 살펴봐야 한다. 
이 함수는 `ReadFile`함수, `VirtualAlloc` 함수 뒤에 나오며, 이 함수를 실행한 이후 파일을 잠그고 어떤 내용물을 쓰며, 다시 파일을 잠금 해제하는 함수들인 `LockFile`, `WriteFile`, `UnlockFile` 함수가 나오기 때문에 파일의 내용물을 읽고 그 내용물을 암호화 알고리즘으로 암호화 해서 `WriteFile` 함수에서 그 내용을 기록할 수 있도록 하는 함수로 추측된다. 

샘플 파일은 다음과 같다. 
![[Pasted image 20250826154159.png]]
앞에서 나왔던 함수들을 실행하고, `ReadFile` 함수를 실행해 성공해서 반환값이 0이 아니면 `ragnar_locker.5926B0` 함수를 실행하는데, 이 때 이 함수에 들어가는 인자들을 보면 [[x64dbg로 Ragnar Locker 코드 흐름 파악 (동적 분석)#ragnar_locker.5931D0 함수 분석 (2번 수행, 난수 생성)|난수 생성]] 과정에서 생성한 40 byte와 32 byte를 Parameter로 받는다. 
![[Pasted image 20250826161933.png]]
`ragnar_locker.5926B0`함수를 살펴보면 별도의 함수 호출 없이 산술연산과 비트연산 정도로만 구성된 것을 확인할 수 있어 해당 연산은 XOR 연산이 많이 사용되지 않은 것으로 보아 암호화 알고리즘 연산은 아닌 것으로 판별되었다. 

그 다음, 드디어 `ragnar_locker.592310` 함수에 대해 분석해 보자. 
`ragnar_locker.592310` 함수는 `SetFilePointerEx`, `ReadFile`, `SetFilePointerEx` 함수를 순서대로 실행한 이후 호출된다. 
함수 안으로 들어가 보면, 함수 에필로그부터 함수 프롤로그까지 별도의 함수 호출 없이 `mov, rol, ror, xor, add, cmp, shr, inc` 등의 연산만 사용해 입력받은 Parameter에 대한 연산을 실행한다. 
![[Pasted image 20250827134041.png]]
이 사진에서 특별히 주의깊게 볼 것이 있는데, `rol` 명령어와 `ror` 명령어 부분이다. 
해당 명령어를 사용하는 과정에서 `rol ebx,7`, `rol esi,9`, `rol edx,D`, `ror ecx,E` 등 7, 9, D, E를 사용하는 비트 시프트 명령어가 분기 명령어를 무시하면 총 **8번** 등장한다. 
이 때, E는 `ror` 명령어를 사용하는데, 이는 비트 시프트 연산자의 특성 때문이다. 
비트 시프트 연산에서 왼쪽 순환 시프트와 오른쪽 순환 시프트는 서로 반대로 회전하며, 다음과 같은 관계가 있다. 
$$x <<< r = x >>> (32-r)$$
즉, 32비트 정수에서 왼쪽으로 r비트 순환 시프트한 결과는 오른쪽으로 32-r 비트 순환 시프트한 결과와 동일하다. 
따라서, Salsa20의 Quarter Round에서 a ^= (d + c) <<< 18은 a ^= (d + c) >>> (32-18) = 
a ^= (d + c) >>> 14와 동일하다. 즉, 위의 사진에서 보는 명령어는 Salsa20의 Quarter Round 함수와 동일하다고 판단할 수 있다. 
Salsa20 알고리즘의 위키백과 문서[^2]를 보면 Odd Round와 Even Round를 전부 합쳐 반복문 한 번 당 Quarter Round가 8번 실행되는데, 이는 방금 전 관찰했던 결과와 동일하다. 
```cpp
// Salsa20의 라운드 구조 코드
void salsa20_block(uint32_t out[16], uint32_t const in[16])
{
	int i;
	uint32_t x[16];
	
	for (i = 0; i < 16; i++) 
	{
		x[i] = in[i]; 
	}
	
	// 10 loops X 2 rounds/loop = 20 Rounds
	for (i = 0; i < 20; i += 2)
	{
		// Odd round
		QR(x[0], x[4], x[8], x[12]); // column 1
		QR(x[5], x[9], x[13], x[1]); // colomn 2
		QR(x[10], x[14], x[2], x[6]);// colomn 3
		QR(x[15], x[3], x[7], x[11]);// colomn 4
		
		// Even round
		QR(x[0], x[1], x[2], x[3]);     // row 1
		QR(x[5], x[6], x[7], x[4]);     // row 2
		QR(x[10], x[11], x[8], x[9]);   // row 3
		QR(x[15], x[12], x[13], x[14]); // row 4
	}
	
	for (i = 0; i < 16; ++i)
		out[i] = x[i] + in[i];
}
```
그 다음으로 알아보아야 할 것은 Salsa20의 Initial State를 구성하는 방법인데, 이는 위에서 분석했던 `ragnar_locker.5926B0` 함수가 0x28 byte, 0x20 byte 두 개를 받아 Initial State를 구성하는 함수라는 것을 유추할 수 있었다. 
그러나, 어떻게 Initial state를 만드는지는 아직 어셈블리에 대한 이해가 완벽하지 않아 어떤 방식으로 만드는지는 확인하지 못했다. 

[^1]: https://crypto.stackexchange.com/questions/11182/security-considerations-on-expand-32-byte-k-magic-number-in-the-salsa20-family
[^2]: https://en.wikipedia.org/wiki/Salsa20