관련 문서: [[x64dbg로 Ragnar Locker 코드 흐름 파악 (동적 분석)]]

위의 문서에서 이어지는 내용으로, `ragnar_locker.591950`함수는 파일을 하나하나 돌면서 암호화할 대상 파일을 탐색하는 함수로 추측되며, 그 안에서 호출되는 `ragnar_locker.591490`함수와 해당 함수가 호출하는 **피호출자(함수가 호출하는 또 다른 함수)가 해당 파일을 암호화하는 함수로 추측된다.**

`ragnar_locker.591490`함수가 호출하는 피호출자는 순서대로 다음과 같다. 
1. [CreateFileW](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-createfilew) 함수
2. [GetFileSizeEx](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-getfilesizeex) 함수
3. `ragnar_locker.5922B0` 함수
4. `ragnar_locker.597480` 함수
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

---
