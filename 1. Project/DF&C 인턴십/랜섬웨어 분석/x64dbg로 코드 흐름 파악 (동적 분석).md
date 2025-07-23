x64dbg에 라그나로커 랜섬웨어를 물려 놓고 F9 키를 3번 정도 누르면 함수 프롤로그를 발견할 수 있다. 
```
push ebx
mov ebx, esp
sub esp, 8
```
이제 [[x64dbg 사용법]]에서 본 것처럼 중단점을 찍고, 한 줄씩 내려가면서 새로운 함수가 보이면 중단점을 찍고, 그 안으로 들어가거나 하면서 분석해 보자. 
그 전에 먼저 어떤 API가 사용되는지를 알아야 할 것 같아서 로그 창에서 다음과 같은 dll들이 사용되는 것을 확인했고, 그 다음으로 x64dbg를 사용해 랜섬웨어의 전체적인 흐름을 분석해 봤다.
>[!list]- 사용된 dll들
kernel32.dll
kernelBase.dll
user32.dll
gdi32.dll
msvcp_win.dll
ucrtbase.dll
advapi32.dll
msvcrt.dll
sechost.dll
rpcrt4.dll
shell32.dll
WinTypes.dll
combase.dll
shlwapi.dll
crypt32.dll
psapi.dll

먼저 쭉 내려가다가 `call ragnar_locker.3F74C0`라는 함수를 만날 수 있고, 그 곳에 중단점을 찍고 F7을 눌러 그 함수로 들어가 분석해 보자. ![[Pasted image 20250723143853.png]]
그러면, 다음과 같이 그 밑에 루프가 있는 것을 발견할 수 있는데, 이 루프는 약 3번 정도 돌다가 방금 중단점을 설정해 놓은 그 곳 바로 밑으로 빠져나오는 것을 확인할 수 있다. 
이후 `push esi, push edi`를 사용해 데이터의 주소와 목적지의 주소를 스택에 저장하고 나서 `call ragnar_locker.3F1CF0` 명령어를 사용해 해당 함수로 넘어간다. 

![[Pasted image 20250723144805.png]]
그러면 이렇게 엄청나게 많은 귀찮은 코드가 있는데, 분기나 함수 호출 등 의미있는 코드가 나올 때까지 계속 F8으로 넘어가 보자. 
계속 넘어가고 또 넘어가 보면...
![[Pasted image 20250723145017.png]]
다음처럼 [GetLocaleInfow](https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getlocaleinfow)함수를 사용해 사용자의 국가, 언어 등을 받아와 EAX 레지스터에 영문 문자열로 저장하며, 이를 아래의 중단점이 2개 쳐져 있는 반복문을 이용해 여러 나라의 언어와 비교한다. 

![[Pasted image 20250723150102.png]]
이후 라그나로커 시작 지점으로 다시 돌아와 컴퓨터 자체의 이름과 Username을 받고 다음 함수를 실행할 준비를 한다. 

`ragnar_locker.3F21C0`로 넘어가 보면 [VirtualAlloc](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc)함수를 사용해 현재 프로세스의 가상 메모리를 할당하며, [RegOpenKeyExW](https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regopenkeyexw)함수를 이용해 [[레지스트리]]의 키를 연다. 
그리고 나서 [RegQueryValueExW](https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regqueryvalueexw)함수를 사용해 열었던 [[레지스트리]] 키와 연관된 지정된 값/이름에 대한 유형과 데이터를 검색하고, [RegCloseKey](https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-regclosekey)함수를 사용해 열었던 [[레지스트리]]의 키를 닫는다. 
![[Pasted image 20250723153424.png]]
이후 해당 작업이 전부 끝나고 나면 이 함수를 호출하기 이전의 메인함수로 돌아가 다음 코드를 시작하며, 이후의 코드에서 `push eax`가 실행될 때마다 각각 사용자의 이름(USERNAME), 컴퓨터의 이름이 eax 레지스터에 push되는 것을 확인할 수 있었다. 
![[Pasted image 20250723154820.png]]

그 다음은 `ragnar_locker.3F2240` 함수를 실행해 보자. 스크린샷에서도 보이듯이 esi 레지스터에는 현재 Windows 버전이, eax 레지스터에는 컴퓨터의 이름이, edi 레지스터에는 시리얼 코드 비슷한 문자열이 각각 보이는 것을 확인했다. ![[Pasted image 20250723160926.png]]
이 코드에서는 앞서 나왔던 VirtualAlloc 함수,`lstrlenW` 함수와 `wsprintfW`함수가 사용되었는데, 이는 각각 지정된 문자열의 길이를 결정하고, 지정된 버퍼에 서식이 지정된 데이터를 쓰는 함수이다. 
따라서 `ragnar_locker.3F2240` 함수는 `lstrlenW`함수로 입력된 문자열의 길이를 계산하고, 루프에서 정의된 반복문대로 움직이며 간단한 해시값이나 무결성 체크 등의 연산을 수행하는 함수로 추측된다. 
그 뒤에도 4번 정도 `ragnar_locker.3F2240`함수를 더 수행하는 코드가 있으며, 이 코드를 수행한 뒤, `wsprintfW`함수를 호출해 ESP에 `A6A64009-6FAB6FDA-780E09FA-818CD995-10352210`의 해시값 비슷한 것을 저장한다. 

그 밑으로 수많은 함수가 사용되고 있는, Ragnar Locker 랜섬웨어에서 메인 기능을 담당하는 것처럼 보이는 코드가 등장한다. 하나하나 분석해 보자. 
![[Pasted image 20250723163808.png]]
먼저, [CreateEventW](https://learn.microsoft.com/ko-kr/windows/win32/api/synchapi/nf-synchapi-createeventw), [GetLastError](https://learn.microsoft.com/ko-kr/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror), [GetCurrentProcess](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocess), [TerminateProcess](https://learn.microsoft.com/ko-kr/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminateprocess), [CloseHandle](https://learn.microsoft.com/ko-kr/windows/win32/api/handleapi/nf-handleapi-closehandle) 함수들을 사용하고, 함수의 반환값을 특정 값과 비교하는 방식으로 현재 랜섬웨어 프로세스를 관리하며, `while`반복문을 0xFAE9과 비교하는 방식으로 인덱스를 1씩 늘려 가며 (inc esi, cmp esi, FAE9) 동작한다.
그 뒤에 `ragnar_locker.3F31D0`와 `ragnar_locker.3F31E0` 함수가 2개씩 나온다. 
하나씩 알아보자. 
