## 동작 분석 Warm-Up
x64dbg에 랜섬웨어를 물려 놓고 F9 키를 여러 번 누르면 [[함수 프롤로그 & 에필로그|함수 프롤로그]]를 발견할 수 있다. 

`다음을 찾기 > 모든 모듈 > 모듈간 호출`로 `Crypt`가 들어가는 암호화 관련 함수를 찾아보려 했으나, 유의미한 결과가 나오지 않아 랜섬웨어 관련 핵심 함수에 뭔가 난독화 같은 게 적용되었다고 추측할 수 있었다. 
![[Pasted image 20251030173054.png]]
이는 DLL(Dynamic Link Library) 파일의 사용을 봐도 알 수 있듯이, 암호화 관련 DLL은 딱히 정의되지 않았던 것을 확인할 수 있다. 

반면, `File`로 필터링한 결과는 다음과 같이 정상적으로 나오기에 이 파일은 암호화 관련 함수, DLL 등 정적 분석에서 확인할 수 있는 것을 난독화하여 안티 바이러스 엔진을 회피한다. 
따라서, 랜섬웨어를 정상적으로 작동시키기 위해서는 **악성 행위 이전에 난독화를 해제하는 코드가 있을 것이라 예측할 수 있다.**

x32dbg 대신 x64dbg를 사용했기 때문에 전반적으로 뭔가 [[Ragnar Locker 랜섬웨어 분석]] 때보다 exe 파일 뒤의 숫자가 더 길어진 것을 확인할 수 있다. 
```assembly
sub rsp, 28
call lockbit.7FF7D30D0BC4
add rsp, 28
jmp lockbit.7FF7D30D06E0
```
(앞으로는 `lockbit.` 뒤의 중복되는 것은 생략하던가 해야겠다. 아마 `7FF7D3` 부분)
이제 해당 function call 또는 system call 지점에 중단점을 찍고, 한 줄씩 내려가면서 분석해 보자. 

## 메인 함수 분석
랜섬웨어의 동작이 시작되는 함수인 `lockbit.0CC030` 함수에 도착해 F7으로 들어가 보면 다음과 같은 어셈블리 명령어들이 있는 것을 확인할 수 있다. 
![[Pasted image 20251030135502.png]]
이 사진은 나중에 발표를 위해 캡처해 두고, F7과 F8을 사용해 코드를 계속 실행하다 보면 `lockbit.0B3570` 함수 내부의 `lockbit.0B2F00` 함수에서 추가적인 함수 호출이 거의 없이 특정 코드를 반복 실행하는 것을 확인할 수 있었다. (이 때, `lockbit.0B2F00`의 인자 전달은 없었다.)
![[Pasted image 20251030142009.png]]

여기서 역난독화 연산이 진행된다. (뒤에서도 여러 번 진행된다.)

### `kernel32.dll` 접근: `lockbit.0B2F00`
 KISA에서 LockBit를 분석한 자료에 의하면, 다음과 같이 역난독화가 진행된다. 
![[Pasted image 20251108211716.png]]

이 때 $a$와 $b$ 값을 전부 찾을 수 있는데, 왜냐하면 $mod\ 127$ 연산에서 127 = `0x7F`이므로 `0x7F` 근처에 나와 있는 값을 찾으면 되기 때문이다. 
*(물론, 발표할 때는 어떻게 확신했냐는 질문에 연산 구조가 나중에도 반복되는 것 같아 조사해 봤더니 이렇게 동작했고, 역산해 보니 어떤어떤 역할을 하는 것으로 드러났다고 하자..)*

![[Pasted image 20251108213937.png]]
이 사진에서는 함수 프롤로그가 등장한 뒤 연산을을 진행할 배열의 값을 얻기 위해 다음처럼 26개의 값들을 스택의 기준 포인터가 되는 rbp 레지스터로 시작하는 배열에 집어넣고, 밑의 사진처럼 반복문을 `0x1A` = 26번 각각에 대하여 전부 돌리면서 연산한다. 
![[Pasted image 20251108214333.png]]
여기서 역난독화 핵심 연산이 진행된다. r8 레지스터가 반복문을 제어하는 인덱스가 되며, r8 레지스터의 값이 `0x1A` = 26이 될 때까지 1씩 증가시키며 다음의 연산을 수행한다. 
```assembly
mov eax, 16
sub eax, ecx
imul ecx, eax, 23 
mov eax, 81020409
imul ecx
add edx, ecx
sar edx, 6
mov eax, edx
shr eax, 1F
add edx, eax
imul eax, edx, 7F
sub ecx, eax
mov eax, 81020409
add ecx, 7F
imul ecx
add edx, ecx
sar edx, 6
mov eax, edx
shr eax, 1F
add edx, eax
imul eax, edx, 7F
sub ecx, eax
```
이 때, 모르는 두 개의 명령어가 있는데, 한 번 구글링으로 알아보자.
- `imul` 명령어
	- 부호 있는 곱셈에 사용
	- `imul source`: eax 레지스터의 값과 `source`를 곱한 값을 `source`에 저장
	- `imul register1, source`: `register1`의 값과 `source`을 곱한 값을 `register1`에 저장
	- `imul register1, source1, source2`:`source1`과 `source2`를 곱한 값을 `register1`에 저장
- `sar` 명령어
	- 산술 오른쪽 시프트에 사용 (Shift Arithmetic Right)
	- 부호 있는 숫자: 양수의 경우, 최상위 비트가 0이므로 0으로, 음수는 반대로 1로
	- `shr`(논리적 오른쪽 시프트)과의 차이: `shr`은 항상 0으로, `sar`은 MSB 복사
	- 결과: 부호 있는 정수를 2의 거듭제곱으로 나누는 것과 같은 효과

이를 참고해서 어셈블리 명령어를 해석해 수식으로 나타낸 결과는 다음과 같다. 
$$ret = (35(22-x) mod\ 127)+127)mod\ 127$$

해당 역난독화를 실행하면서 관찰해 보면, 아래 사진에서도 알 수 있듯이 덤프 창에서 `kernel32.dll`을 rbp 레지스터가 가리키는 주소 근처로 계산(?) 해 내는 것을 확인할 수 있다. 
![[Pasted image 20251109014320.png]]
이는 위에서 유추했던 내용대로라면`kernel32.dll`에 접근해 내부에 구현된 함수인 `LoadLibraryA`와 `GetProcAddress` 함수를 사용해 특정 DLL을 로드하고 DLL에서 암호화 관련 함수의 주소를 가져오는 행위를 하기 위해서 실행되었다고 유추해 볼 수 있다. 

 `while` 연산이 끝난 뒤, 그 함수 내부에서도 `lockbit.0B2CA0`를 호출하며, 그 내부에서 실행을 마치고 마지막에`lockbit.0B4870` 함수를 호출한다.

`lockbit.0B4870` 함수 내부에서는 추가적인 함수 호출은 없었으며, 하단의 사진과 같이 특정 값인 `5BD1E995`를 기반으로 XOR, 오른쪽 비트 시프트 등의 연산을 여러 번 하는 것을 확인했다. 
![[Pasted image 20251030143936.png]]

### 자식 프로세스 생성: `lockbit.0B2BB0`
이후, `lockbit.0B2BB0` 함수를 호출한다. `lockbit.0B2BB0` 함수를 동적으로 실행하면서 관찰해 보면 다음과 같은 여러 함수들이 rax 레지스터나 rcx 레지스터에 저장되는 것을 확인할 수 있다.

- ActivateActCtx
- ActivateActCtxWorker
- ActivatePackageVirtualizationContext
- AddAtomA
- AddAtomW
- AddConsoleAliasA
- AddConsoleAliasW
- AddDllDirectory

[예시 사진]
![[Pasted image 20251109023812.png]]
계속 반복문을 실행하다 보면, `CreateFile`이나 `CreateProcessA` 등의 함수도 나온다. 단, `Crypt`로 시작하는 함수들은 나오지 않으며, 이를 통해 해당 함수는 자식 프로세스를 생성하는 함수에 더 가깝다는 것을 유추할 수 있다. 

`lockbit.0B2BB0` 함수의 실행이 끝나면 갑자기 `lockbit....`으로 되어 있던 것이 `LoadLibraryA` 함수로 변경된다. 그리고 변경된 함수를 덤프해 따라가 보면 두 번째 사진과 같다.

[실행 종료 이전]
![[Pasted image 20251109024625.png]]
[실행 종료 이후]
![[Pasted image 20251031023027.png]]

이 `LoadLibraryA` 함수는 DLL을 이 프로세스의 주소 공간에 로드하는 함수로, 로드할 모듈의 이름을 매개변수로 받아서 동작한다. 
이후, `lockbit.0B4650` 함수를 실행하며, 계속 실행하다가 `lockbit.0B38D0` 함수를 수행하고 나서 그 다음 명령어를 실행해 보면 EAX 레지스터에 본 적 없던 DLL인 `advapi32.dll`이 Import된 것을 확인할 수 있으며,(왼쪽) 해당 DLL로 들어가 보면 암호화 관련 함수들이 Import된 것을 확인했다. (오른쪽)
![[Pasted image 20251031052626.png|350]]![[Pasted image 20251031052653.png|350]]

이후`call rax` 명령을 통해 RAX 레지스터에 있는 함수인 `CryptoAcquireContextA` 함수를 호출한다. 
### 암호화 관련: `lockbit.0B4650`
이후, `lockbit.0B4650` 함수를 다시 호출하는데, 매개변수로 16, 55, `0x70D2C0E4`를 받아 동작한다. 이는 함수 스택 창을 열어서 확인해 볼 수 있다. (기본값(x64 fastcall) 이 부분)
![[Pasted image 20251109042816.png]]
이 때 `mov dword ptr ss:[rsp+F0], eax`부터 해당 줄 포함 4줄은 데이터를 복호화하기 위한 AES Key BLOB의 정보를 구성하는 구조체 변수로, 첫 번째 숫자인 208은 BLOB 형식과 키가 사용하는 알고리즘을 나타내는 PUBLICKEYSTRUC 구조체, 두 번째 숫자인 6610은 사용할 암호 알고리즘, 세 번째 숫자인 20은 키의 길이인 0x20, 즉 32-byte를 나타낸다. 

따라서, 사진에서도 볼 수 있듯이, 이번의 `lockbit.0B4650`함수는 다른 용도인 `CryptImportKey` 함수의 용도로 사용됨이 드러났다. *(설명할 때는 이 실행 결과 먼저 보여주고, 그 다음 Key BLOB의 설명을 하자)*
![[Pasted image 20251031054808.png]]

그 다음도 다시 이 함수를 호출하며, 이번에는 16, `0xA1F10DB0`, 111을 받아 움직인다. 이번에도 함수의 인자는 함수 스택 창에서 확인할 수 있다. 
![[Pasted image 20251109043114.png]]
이번에는 `CryptSetKeyParam` 함수를 사용하여 IV의 값을 설정한다. 이 때 암호화된 랜섬노트에 하드코딩된 상태로 저장된 IV 값을 고정된 IV로 사용하며, Crypto API의 Default 운영모드는 CBC이기 때문에 복호화 알고리즘은 자연스럽게 AES-256-CBC로 설정된다. 
![[Pasted image 20251031055818.png]]

이후, 또 다시 16, 112, `0xDC0D9528`을 인자로 받는 동일한 함수가 호출되며 **(왼쪽)**, 이 함수는 `CryptDecrypt` 함수를 사용하여 첫 번째와 두 번째 `lockbit.0B4650` 함수 호출의 결과를 바탕으로 해서 랜섬노트 데이터를 AES-256-CBC 알고리즘으로 복호화한다.**(오른쪽)** 
![[Pasted image 20251109043312.png|350]]![[Pasted image 20251031060957.png|350]]

복호화된 랜섬노트 데이터는 다음과 같다. (RSI 레지스터가 가리키는 곳에 있다)
![[Pasted image 20251031061136.png]]

이후, 특정 주소에 저장되어 있는 배열을 반복문을 통해 순회하며 0x78과 XOR 연산해 LockBit 랜섬웨어에 감염된 파일의 확장자인 `.fb7c204e`를 획득한다.
![[Pasted image 20251031062848.png]]

그렇다면, 이 때 서로 다른 인자를 넣었을 때 동일한 `lockbit.0B4650` 함수에서 각각 다른 `Crypt` 로 시작하는 함수가 호출된 이유가 무엇일까? 분석해 보자. 



---
## 더 자세하고 추가적인 분석
[[LockBit 3.0 파일 암호화 알고리즘 파악]]
[[LockBit 3.0 랜섬웨어 복호화]]