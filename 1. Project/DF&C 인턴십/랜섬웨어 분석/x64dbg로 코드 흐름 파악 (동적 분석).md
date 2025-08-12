x64dbg에 라그나로커 랜섬웨어를 물려 놓고 F9 키를 3번 정도 누르면 [[함수 프롤로그 & 에필로그|함수 프롤로그]]를 발견할 수 있다. 
```
push ebx
mov ebx, esp
sub esp, 8
```
이제 [[x64dbg 사용 방법]]에서 본 것처럼 중단점을 찍고, 한 줄씩 내려가면서 새로운 함수가 보이면 중단점을 찍고, 그 안으로 들어가거나 하면서 분석해 보자. 

그 전에, 우리가 넘어갔던`ntdll.dll`은 랜섬웨어 등의 악성코드에서 다음 이유로 사용한다. 
1. **탐지 우회**
	보통 정상적인 애플리케이션은 `kernel32.dll`이나 `user32.dll` 등의 고수준 API를 사용한다. 
	그러나 악성코드는 보안 솔루션의 고수준 API 감시를 피하기 위해 `ntdll.dll` 등의 Native API를 직접 호출한다. 
2. **메모리 조작 및 인젝션**
	악성코드는 종종 다른 프로세스를 통제하기 위해 `ntdll.dll`의 함수를 사용한다.
	- `NtAllocateVirtualMemory`: 메모리 공간 확보
	- `NtWriteVirtualMemory`: 메모리에 악성 코드 쓰기
	- `NtCreateThreadEx`: 원격 스레드 생성 (코드 실행)
	이런 방식은 **DLL 인젝션**, **Reflective DLL Injection**, **Process Hollowing** 같은 기술에 사용된다.
3. **Hook 우회 및 API 패치 탐지**
	보안 솔루션은 종종 고수준 API에 API Hooking을 걸어 악성 행위를 탐지한다. 그러나 `ntdll.dll`의 Native API는 상대적으로 덜 감시되기 때문에 악성코드는 이를 통해 API Hooking을 우회한다. 
	- `NtQuerySystemInformation`을 사용해 시스템 정보나 프로세스 리스트를 가져와 보안 솔루션을 탐지할 수 있음
	- `NtProtectVirtualMemory`를 사용해 메모리 보호 속성을 변경하여 탐지 회피
4. **Syscall 직접 호출**
	고급 악성코드는 `ntdll.dll`을 거치지 않고 직접 시스템 호출을 수행하기도 한다. 하지만 대부분의 악성코드는 `ntdll.dll`의 함수에서 Syscall 번호를 추출해 이를 기반으로 직접 호출한다. 
	이는 보안 솔루션이 `ntdll.dll`을 모니터링하더라도 탐지하지 못하게 한다. 

다시 돌아가서, 랜섬웨어를 동적 분석해 보자. 
먼저 쭉 내려가다가 `call ragnar_locker.5974C0`라는 함수를 만날 수 있고, 그 곳에 중단점을 찍고 F7을 눌러 그 함수로 들어가 분석해 보자. ![[Pasted image 20250726134518.png]]
그러면, 다음과 같이 그 밑에 루프가 있는 것을 발견할 수 있는데, 이 루프는 약 3번 정도 돌다가 방금 중단점을 설정해 놓은 그 곳 바로 밑으로 빠져나오는 것을 확인할 수 있다. 
이후 `push esi, push edi`를 사용해 데이터의 주소와 목적지의 주소를 스택에 저장하고 나서 `call ragnar_locker.591CF0` 명령어를 사용해 해당 함수로 넘어간다. 

![[Pasted image 20250726134659.png]]
그러면 이렇게 엄청나게 많은 귀찮은 코드가 있는데, 분기나 함수 호출 등 의미있는 코드가 나올 때까지 계속 F8으로 넘어가 보자. 
계속 넘어가고 또 넘어가 보면...
![[Pasted image 20250726134823.png]]
다음처럼 [GetLocaleInfow](https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getlocaleinfow)함수를 사용해 사용자의 국가, 언어 등을 받아와 EAX 레지스터에 영문 문자열로 저장하며, 이를 반복문을 이용해 여러 나라의 언어와 비교한다. 이때 비교하는 언어는 Azerbaijani, Armenian, Belorussian, Kazakh, Kyrgyz, Moldavian, Tajik, Russian, Turkmen, Uzbek, Ukrainian, Georgian 총 12개이다. (해당 언어들은 소련의 일부였다는 공통점이 있다)
이후 함수 에필로그를 통해 함수 밖으로 나간다. 

![[Pasted image 20250726135008.png]]
이후 라그나로커 시작 지점으로 다시 돌아와 컴퓨터 자체의 이름과 Username을 받고 다음 함수를 실행할 준비를 한다. 

`ragnar_locker.5921C0`함수로 넘어가 보면 [VirtualAlloc](https://learn.microsoft.com/ko-kr/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc)함수를 사용해 현재 프로세스에 추가적인 행위를 하기 위한 가상 메모리를 할당하며, [RegOpenKeyExW](https://learn.microsoft.com/ko-kr/windows/win32/api/winreg/nf-winreg-regopenkeyexw)함수를 이용해 [[레지스트리]]의 키를 연다. 
그리고 나서 [RegQueryValueExW](https://learn.microsoft.com/ko-kr/windows/win32/api/winreg/nf-winreg-regqueryvalueexw)함수를 이용해 열었던 레지스트리 키와 연관된 지정된 값/이름에 대한 유형과 데이터를 검색하고, [RegCloseKey](https://learn.microsoft.com/ko-kr/windows/win32/api/winreg/nf-winreg-regclosekey)함수를 사용해 열었던 레지스트리의 키를 닫는다. 
![[KakaoTalk_20250805_221734510_01.png]]
이후 해당 작업이 전부 끝나면 이 함수를 호출하기 이전의 메인 함수로 돌아가 다음 코드를 시작하며, 이후의 코드에서 `push eax`가 실행될 때마다 각각 사용자의 이름(USERNAME), 컴퓨터의 이름이 EAX 레지스터가 가리키는 주소의 값에 push되는 것을 확인할 수 있었다. 
![[KakaoTalk_20250805_221734510.png]]

이제`ragnar_locker.592240` 함수를 실행해 보자. 스크린샷에서도 보이듯이 esi 레지스터가 가리키는 메모리 주소의 값에는 현재 Windows 버전이, eax 레지스터가 가리키는 메모리 주소의 값에는 컴퓨터의 이름이, edi 레지스터가 가리키는 메모리 주소의 값에는 시리얼 코드 비슷한 문자열이 각각 보이는 것을 확인했다. 
이 코드에서는 앞서 나왔던 VirtualAlloc 함수와`lstrlenW` 함수, `wsprintfW`함수가 사용되었는데, 이는 각각 지정된 문자열의 길이를 결정하고, 지정된 버퍼에 서식이 지정된 데이터를 쓰는 함수이다. 
![[Pasted image 20250726140128.png]]
따라서 `ragnar_locker.592240` 함수는 `lstrlenW`함수로 입력된 문자열의 길이를 계산하고, 루프에서 정의된 반복문대로 움직이며 간단한 해시값이나 무결성 체크 등의 연산을 수행하는 함수로 추측된다. 
그 뒤에도 4번 정도 `ragnar_locker.592240`함수를 더 수행하는 코드가 있으며, 이 코드를 수행한 뒤, `wsprintfW`함수를 호출해 ESP 레지스터가 가리키는 메모리 주소의 값에 `A6A64009-6FAB6FDA-780E09FA-818CD995-10352210`의 해시값 비슷한 것을 저장한다. 
더 정확히는, 컴퓨터 이름, 사용자 이름, 컴퓨터의 guid, Windows 버전, combined_info의 순서대로 해시를 계산해서 위의 해시값의 형태로 저장한다. (출처: https://www.cybereason.com/blog/threat-analysis-report-ragnar-locker-ransomware-targeting-the-energy-sector)
![[Pasted image 20250812175042.png]]
![[Pasted image 20250726140355.png]]

그 밑으로 수많은 함수가 사용되고 있는, Ragnar Locker 랜섬웨어에서 메인 기능을 담당하는 것처럼 보이는 코드가 등장한다. 하나하나 분석해 보자.
![[Pasted image 20250726140432.png]]
먼저, [CreateEventW](https://learn.microsoft.com/ko-kr/windows/win32/api/synchapi/nf-synchapi-createeventw), [GetLastError](https://learn.microsoft.com/ko-kr/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror), [GetCurrentProcess](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocess), [TerminateProcess](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminateprocess), [CloseHandle](https://learn.microsoft.com/ko-kr/windows/win32/api/handleapi/nf-handleapi-closehandle) 함수들을 사용하고, 함수의 반환값을 특정 값과 비교하는 방식으로 현재 랜섬웨어 프로세스를 관리하며,
앞에서 계산했던 해시값이 이어진 문자열을 이벤트 이름으로 사용한다. 
[GetCurrentProcess](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminateprocess)함수로 현재 실행 중인 프로세스들을 받아 와 [TerminateProcess](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminateprocess)함수를 사용해 현재 실행 중인 프로세스들을 강제로 정지한다. 해당 작업은`while`반복문을 0xFAE9과 비교하는 방식으로 인덱스를 1씩 늘려 가며 (inc esi, cmp esi, FAE9) 동작한다.
그 뒤에 `ragnar_locker.5931D0`와 `ragnar_locker.5920E0` 함수가 2개씩 나온다. 
하나씩 알아보자. 

---
### ragnar_locker.5931D0 함수 분석 (2번 수행)
먼저 `ragnar_locker.5931D0`함수를 call하는 지점에 중단점을 걸고 F7으로 들어가서 분석해 보면 그 안에서도 함수 에필로그 부분까지 여러 함수들을 추가로 호출한다. 
호출하는 함수는 다음과 같다. 
1. [CryptAcquireContextW](https://learn.microsoft.com/ko-kr/windows/win32/api/wincrypt/nf-wincrypt-cryptacquirecontextw) 함수: 특정 CSP(암호화 서비스 공급자) 안의 특정 키 [[컨테이너]]에 대한 핸들을 획득하는 데 사용하며, 이 핸들은 선택한 CSP를 사용하는 CryptoAPI 함수 호출에 사용된다. 해당 함수는 [[CNG (Cryptography Next Generation)|CNG]]의 [BCryptOpenAlgorithmProvider 함수](https://learn.microsoft.com/ko-kr/windows/win32/api/bcrypt/nf-bcrypt-bcryptopenalgorithmprovider)와 비슷하게 암호화, 난수 생성 등을 위한 준비 단계에 해당한다.
2. [CryptGenRandom](https://learn.microsoft.com/ko-kr/windows/win32/api/wincrypt/nf-wincrypt-cryptgenrandom) 함수: 지정한 버퍼에 지정한 바이트 수 만큼의 난수를 넣어 반환한다. 
3. [CryptReleaseContext](https://learn.microsoft.com/ko-kr/windows/win32/api/wincrypt/nf-wincrypt-cryptreleasecontext) 함수: CSP의 키 컨테이너의 핸들을 해제한다. 이 함수는 CNG의 [BCryptCloseAlgorithmProvider 함수](https://learn.microsoft.com/ko-kr/windows/win32/api/bcrypt/nf-bcrypt-bcryptclosealgorithmprovider)와 비슷한 역할을 수행한다. 
4. `ragnar_locker.597240`함수
	![[Pasted image 20250805165437.png]]
	별도의 함수 호출 없이 EAX 레지스터가 가리키는 메모리 주소에 4바이트 값을 16번, 총 64바이트의 값을 저장하며, 각각의 값은 사진과 같이 실행파일 내에 하드코딩 되어 있었다. 
5. `ragnar_locker.5972F0`함수
	![[Pasted image 20250805165946.png]]
	아직 어셈블리어에 대한 이해가 완벽하지 않아 이 함수는 별 다른 역할을 하지 않는 함수처럼 보였다. 
6. `ragnar_locker.596F30`함수
	이 함수에서는 쭉 내려가다가 다시 여러 함수들을 호출한다. 
	![[Pasted image 20250805171032.png]]
	1. `ragnar_locker.593290` 함수
		![[Pasted image 20250805171542.png]]
		`ret`명령어로 반환하기 전까지 `mov, sub, xor, add`등의 연산을 많이 수행한다. 
	2. `ragnar_locker.5974F0` 함수 (4번 수행)
		![[Pasted image 20250805191659.png]]
		실행하면 `cmp cl,40`, `cmp cl,20`, `mov eax,edx`, `xor edx,edx`, `and cl,1F`, `shr eax,cl`, `ret`의 명령어를 순서대로 수행하고 함수 밖으로 나온다. 
	3. 그 뒤로 `mov` 명령어와 `shr`명령어 두 개만을 사용해 연산을 많이 한 뒤 `ret`명령어로 반환한다.
7. `ragnar_locker.597430`함수 
	![[Pasted image 20250805193525.png]]
	`push`명령어와 `mov`, `and`, `rep`, `shr`, `cld`명령어를 사용해 간단한 연산을 한 뒤 반환한다. 
### ragnar_locker.5920E0 함수 분석 (2번 수행)
![[Pasted image 20250805193812.png]]
이 함수에서는 별도의 함수 호출 없이 함수 에필로그까지 반복문을 사용해 특정 연산을 하고, 특정한 연산이 끝나면 함수 에필로그를 통해 함수 바깥으로 나간다. 
이 때 처음 반복문은 EAX 레지스터의 값이 256이 될 때까지 실행하고, 두 번째 반복문은 ESI 레지스터의 값이 256이 될 때까지 실행한다. 
실행을 전부 끝내면 EAX 레지스터가 가리키는 메모리 주소에 아래 사진처럼 64바이트 값이 저장되어 있다. 
![[Pasted image 20250805195857.png]]

---
그 뒤로 [OpenSCManagerA](https://learn.microsoft.com/ko-kr/windows/win32/api/winsvc/nf-winsvc-openscmanagera)함수를 사용해 해당 컴퓨터에서 [[서비스 제어 관리자]]에 대한 연결을 설정하고 해당 서비스 제어 관리자 데이터 베이스를 연다. 
그 밑으로는 [GetModuleFileNameW](https://learn.microsoft.com/ko-kr/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew)함수로 Ragnar Locker 랜섬웨어 파일 경로를 EAX 레지스터가 가리키는 주소에 저장하며,![[Pasted image 20250806134016.png]]
[PathFindFileNameW](https://learn.microsoft.com/ko-kr/windows/win32/api/shlwapi/nf-shlwapi-pathfindfilenamew)함수, [GetWindowsDirectoryW](https://learn.microsoft.com/ko-kr/windows/win32/api/sysinfoapi/nf-sysinfoapi-getwindowsdirectoryw)함수를 사용해 현재 실행 중인 컴퓨터의 `Windows`디렉터리의 경로를 문자열 형태로 반환해 아래처럼 EAX 레지스터가 가리키는 주소에 저장한다. 
![[Pasted image 20250806134605.png]]

이후 [QueryDosDeviceW](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-querydosdevicew)함수를 사용해 `\\Device\\HarddiskVolume3`경로를 얻어 와서 저장하며, 해당 경로는 Windows 운영체제에서 사용하는 디바이스 경로 중 하나로, 커널 수준에서 물리적 또는 논리적 디스크를 식별하기 위해 사용한다. 
![[Pasted image 20250806140851.png]]
해당 경로는 볼륨 섀도 복사본을 삭제하는 과정에서 나타날 수 있는데, 그 이유는 `C:\`드라이브에 대한 섀도 복사본은 `\\Device\\HarddiskVolumeX`형태로 내부적으로 관리되기 때문이다. 

그 뒤로도 다시 한 번 `OpenProcess`, `TerminateProcess`, `CloseHandle` 함수들과 반복문을 사용해 현재 실행 중인 프로세스들을 강제로 종료한다.
![[Pasted image 20250806142351.png]]
위의 사진에서 해당 반복문을 몇 번 실행하다 나중에는 파란색 분기 안으로 들어가는데, 
이 때 `\\Device\\HarddiskVolume3\\System32\\svchost32.exe`와 `dllhost.exe`, `RuntimeBroker.exe`, `explorer.exe`, `shellhost.exe`등의 응용 프로그램과 프로그램의 스냅샷 등을 순회한다. 
💡이 때 주의할 점은 해당 지점 근처에 [[안티 디버깅]]을 수행하는 부분이 있어 그 밑의 함수인 `ragnar_locker.591000`으로 바로 넘어갈 수 있도록 그 곳에 CPU가 실행할 명령어를 기억하는 EIP 레지스터를 설정해서 바로 넘어가야 한다. 

`ragnar_locker.591000`으로 EIP를 설정하고 F7으로 들어가 분석하면 다음과 같은 사진이 나온다. 
![[Pasted image 20250806155640.png]]
이 함수는 [GetNativeSystemInfo](https://learn.microsoft.com/ko-kr/windows/win32/api/sysinfoapi/nf-sysinfoapi-getnativesysteminfo)함수와 [LoadLibraryW](https://learn.microsoft.com/ko-kr/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw), [GetProcAddress](https://learn.microsoft.com/ko-kr/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress), [GetStartupInfoW](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-getstartupinfow), [CreateProcessW](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw)함수를 사용해 파일 암호화 등을 위해 새로운 프로세스를 생성하고, 이를 위한 DLL들을 동적으로 가져 온다. 

이후 함수 프롤로그를 통해 함수 바깥인 entry로 나가고 나서 그 밑의 함수인 `ragnar_locker.5920E0`으로 들어가면 반복문을 통해 처음에는 EAX 레지스터의 값이 0x100이 될 때까지 `mov byte ptr ss:[ebp+eax-100],al`연산을 하고, 두 번째 반복문에서는 ESI 레지스터의 값이 0x100이 될 때까지 하단의 사진에 나와 있는 연산을 하고 나서 다시 함수 프롤로그를 통해 바깥으로 나온다. 
![[Pasted image 20250806161120.png]]

`ragnar_locker.5920E0`까지 다 수행하고 나면 다음 사진처럼 EAX가 가리키는 주소에 `BEGIN PUBLIC KEY`라는 문구와 함께 어떤 문자열이 나온다. 
![[Pasted image 20250806161334.png]]
이후 `ragnar_locker.591F90`함수로 들어가 보면 [CryptAcquireContextW](https://learn.microsoft.com/ko-kr/windows/win32/api/wincrypt/nf-wincrypt-cryptacquirecontextw) 함수와 [CryptDestroyKey](https://learn.microsoft.com/ko-kr/windows/win32/api/wincrypt/nf-wincrypt-cryptdestroykey)함수를 사용하며, 이후 공개키 암호에 사용되는 함수인 [CryptImportPublicKeyInfo](https://learn.microsoft.com/ko-kr/windows/win32/api/wincrypt/nf-wincrypt-cryptimportpublickeyinfo)함수를 사용해 공개키의 정보를 변환하고 공개키의 핸들을 반환한다. 

`ragnar_locker.5918C0`함수로 들어가면 해당 사진에서 보이는 것처럼 데이터를 조작하거나 복사시에 소스 데이터의 주소가 저장되는 ESI 레지스터가 가리키는 주소에 `BEGIN PUBLIC KEY`가 있는 것으로 보아 RSA 등의 공개키 암호화 알고리즘으로 해당 공개키로 어떤 데이터를 암호화하거나 공개키 그 자체를 암호화하는 것으로 추측된다. 
![[Pasted image 20250806165514.png]]

그런 뒤 `GetComputerNameW`함수로 실행 컴퓨터의 이름을 받아 오고 `ragnar_locker.592240`함수를 실행하고 해당 함수가 끝나면 `lstcpyW`와 `lstcatW`함수를 실행해 랜섬노트의 이름처럼 보이는 `RGNR_818CD995.txt`를 지정하고, [CryptBinaryToStringA](https://learn.microsoft.com/ko-kr/windows/win32/api/wincrypt/nf-wincrypt-cryptbinarytostringa)함수로 바이트 배열을 형식이 지정된 문자열로 변형한다. 
그 다음 `CreateFileW`함수를 이용해 지정된 랜섬노트의 이름으로 랜섬노트 파일을 만들고, 랜섬노트에 들어갈 내용들을 기록한다. ![[Pasted image 20250806171218.png]]

랜섬노트까지 만들고 나서 `ragnar_locker.591950`함수로 들어가 보면 다음과 같은 사진이 나온다. 
![[Pasted image 20250806171609.png]]
[FindFirstFileW](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-findfirstfilew)함수와 [GetFullPathNameW](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-getfullpathnamew)함수가 사진 속에서 같이 사용된 것으로 보아 해당 함수는 반복문을 돌리면서 [FindNextFileW](https://learn.microsoft.com/ko-kr/windows/win32/api/fileapi/nf-fileapi-findnextfilew)등의 함수와 같이 사용해 디렉터리 내의 모든 파일이나 폴더를 반복적으로 검색하여 암호화할 파일을 찾는 함수로 유추할 수 있다. 
이에 대해 실행을 하면서 더 자세하게 분석해 보자.

먼저 `FindFirstFileW`함수로 최상위 폴더인 `C:\`폴더의 파일들부터 차례대로 검색하며, 대소문자 가리지 않고 문자열을 비교하는 `lstrcmpiW`함수로 미리 설정해 둔 문자열, 즉 미리 설정해 둔 파일이나 폴더 명을 찾아 비교하고, 해당 파일의 이름은 `GetFullPathNameW`함수로 찾아 비교한다.
또한, 파일의 확장자를 탐색하는 `PathFindExtensionW`함수로 모든 파일에 대하여 미리 설정해 둔 확장자를 찾아 비교한다. 
미리 설정해 둔 파일, 확장자, 폴더 이름은 다음과 같다. 
1. 파일 이름: 랜섬노트 (`RGNR_818CD995.txt`), `autorun.inf`,`boot.ini`, `bootfont.bin`, `bootsect.bak`, `desktop.ini`, `iconcache.db`, `ntldr`, `ntuser.dat`, `ntuser.dat.log`, `ntuser.ini`, `thumbs.db`
2. 확장자: `db`, `sys`, `dll`, `lnk`, `msi`, `drv`, `exe`
3. 폴더 이름: `Windows`, `Windows.old`, `Tor browser`, `Internet Explorer`, `Google`, `Opera`, `Opera Software`, `Mozilla`, `Mozilla Firefox`, `$Recycle.Bin`, `ProgramData`, `All Users`

이 경우, 랜섬웨어에 걸린 사용자는 랜섬노트가 암호화되지 않고 보여야 하며, 그 외의 이름들은 시스템 동작에 필요한 것들이기 때문에 해당 이름들은 랜섬웨어 실행 시 암호화에서 제외하는 파일, 디렉터리 이름, 확장자로 유추할 수 있다.
>[!example]- 예시 이미지
>![[Pasted image 20250807135225.png]] ![[Pasted image 20250807141736.png]] ![[Pasted image 20250807141841.png]]

이 때 반복문을 돌면서 최상위 디렉터리부터 그 밑의 디렉터리를 하나씩 검색하여 (예: `C:\inetpub`, `C:\PerfLogs` 순서로) 암호화에서 제외할 항목들을 검색한다. 
![[Pasted image 20250807135520.png]]
또한 반복문을 돌며 검사하는 동시에 재귀함수로 `ragnar_locker.591950`함수 자기 자신을 계속 실행하여 `FindNextFileW`함수와 함께 사용해 파일을 계속 탐색하며, 그와 동시에 

계속 실행하다 보면 `ragnar_locker.591950`함수가 호출하는`ragnar_locker.591490`함수로 들어오게 된다. ![[Pasted image 20250807142435.png]]
이 함수에서는 `CreateFileW`함수로 디렉터리 내의 파일을 전부 순회하며 해당 디렉터리와 그 안의 하위 디렉터리에 랜섬노트를 만들고 파일 암호화를 수행하며, 랜섬노트를 만드는 조건은 해당 디렉터리 내의 하위 디렉터리를 전부 순회하여 암호화를 완료했을 때로 추측된다. 
![[Pasted image 20250807150209.png]]
이 때 [[x64dbg]]에서 `덤프`창을 계속 내리다 보면 `0059B000`에 다음과 같이 두 개의 64바이트의 값이 공개키와 같이 메모리 내에 있는 것을 확인할 수 있었다. 
![[Pasted image 20250807162108.png]]
또한, 암호화가 전부 끝나고 나면, 메모장을 실행해 랜섬노트를 모니터에 출력하며, [ExitProcess](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess)함수를 사용해 모든 프로세스와 호출한 프로세스를 전부 종료하고 자기 자신 또한 종료한다. 