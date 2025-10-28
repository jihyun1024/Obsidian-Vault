## 동작 분석 Warn-Up
x64dbg에 LockBit 랜섬웨어를 물려 놓고 F9 키를 여러 번 누르면 [[함수 프롤로그 & 에필로그|함수 프롤로그]]를 발견할 수 있다.
이 때 x32dbg 대신 x64dbg를 사용했기 때문에 전반적으로 뭔가 [[Ragnar Locker 랜섬웨어 분석]] 때보다 더 길어진 것을 확인할 수 있다. 
```assembly
sub rsp, 28
call lockbit.7FF7D30D0BC4
add rsp, 28
jmp lockbit.7FF7D30D06E0
```
(앞으로는 `lockbit.` 뒤의 중복되는 것은 생략하던가 해야겠다. 아마 `7FF7D3` 부분)
이제 해당 function call 또는 system call 지점에 중단점을 찍고, 한 줄씩 내려가면서 분석해 보자. 

먼저, `call lockbit.0D0BC4` 부분을 타고 들어가면 다음과 같으며, 실행하는 코드는 다음과 같다.
![[Pasted image 20251029041431.png]]
여기서는 `GetSystemTimeAsFile`, `GetCurrentThreadId`, `GetCurrentProcessId`, `QueryPerformanceCounter` 함수가 사용된다. 
이 함수들은 현재 시스템의 시간 정보와 현재 Thread, Process의 ID를 얻어 오는 데에 사용되어 랜섬웨어에서는 딱히 중요하지 않은 부분인 것 같아 넘어가도 될 것 같다.

이 함수를 나가서 `jmp 0D06E0`를 실행하면 다음 사진처럼 여러 함수가 밑에 있는 것을 확인할 수 있다. 이 중 가장 먼저 보이는 `lockbit.0D094C`를 먼저 분석해 보자. 
![[Pasted image 20251029042811.png]]

`jmp 0D06E0`를 수행하고 나서 가장 먼저 나오는 함수인 `lockbit.0D094C`의 내부에 들어가 보면 뭔가 연산을 엄청 많이 수행하는 `lockbit.0D0F90` 함수가 존재한다. 
이 때 이 함수를 자세하게 살펴보면 특정 메모리에 있는 값을 참조한 뒤, 각각의 레지스터에 있는 값들과 8자리 16진수 값 여러 개를 XOR 연산하는 것을 확인할 수 있었으며 r8d, r9d, r10d 레지스터의 XOR의 경우는 각각 RBX, RDX, RCX 레지스터의 값과 동일한 값을 XOR 한다는 것을 동적으로 확인할 수 있다. 
![[Pasted image 20251029075357.png]]
(이 사진에서 RAX, RBX, RCX, RDX, RBP 등의 레지스터의 값은 아직 XOR 하기 이전의 값이다)

이후로도 한 번 더 XOR 연산을 수행해 RCX, RDX 등의 레지스터의 값을 새로 지정하고, R8, R9 레지스터의 값을 0으로 설정하고, 이후 몇 번의 연산을 거친 뒤 함수 에필로그로 밖으로 나간다.

그 뒤로 `GetCommandLineA` 등의 여러 함수들을 수행하면서 랜섬웨어 파일이 있는 디렉터리를 한 문자씩 읽어 나간다. 
![[Pasted image 20251029081416.png]]

그 이후 실제 랜섬웨어의 시작 지점인 `lockbit.0CC030` 함수에 도착하게 된다. 여기까지는 랜섬웨어 시작 단계도 아닌 시작 준비 단계이기 때문에, 동작 과정을 소개할 때는 간단하게 소개하거나 아예 건너뛰어도 될 듯 하다. 

## 메인 함수 분석


---
## 더 자세하고 추가적인 분석
[[LockBit 3.0 파일 암호화 알고리즘 파악]]
[[LockBit 3.0 랜섬웨어 복호화]]