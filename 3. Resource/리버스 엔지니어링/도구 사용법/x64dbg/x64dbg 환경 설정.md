
### 🤔x64dbg?

- 실행파일을 동적 분석할 수 있도록 돕는 디버거
    - 동적 분석: 실행 파일을 실행하며 그 행동을 관찰하고 분석하는 방법으로, 이를 통해 프로그램이 실시간으로 어떻게 동작하는지, 시스템 리소스를 어떻게 사용하는지, 어떤 데이터를 주고받는지 등을 확인할 수 있음
- 사용자 친화적 인터페이스와 다양한 플러그인을 지원하며 무료로 사용할 수 있음
- 32bit뿐만 아니라 64bit환경까지 디버깅 가능
- 주요 기능: 중단점 설정, 레지스터 관찰, 메모리 검사

---
### 📲x64dbg 설치

- [x64dbg - Browse /snapshots at SourceForge.net](https://sourceforge.net/projects/x64dbg/files/snapshots/)
- 위 링크에서 `Download Latest Version`을 선택하고 압축 파일을 다운로드하여 해제
    
    - 링크로 이동 시 화면
        
        ![[image1.png]]
        
- 압축 해제된 폴더 중 `release` 폴더를 살펴보면 `x96dbg.exe`라는 프로그램이 있다. 이 프로그램을 실행하면 다음과 같은 화면이 나온다.
	    ![[image 7.png]]

- 프로그램 코드 버전에 따라 32bit용과 64bit용을 실행할 수 있으며, `Install`을 선택하여 x32dbg와 x64dbg를 설치하거나 `release` 폴더 아래의 `x32` 폴더와 `x64`폴더에 x32dbg, x64dbg 실행 파일이 있어 그 실행 파일을 직접 실행해도 된다.

---

### ☃️Snowman Plugin 설치

- x64dbg에는 IDA Pro처럼 C++언어로 디컴파일 하는 기능을 Snowman 플러그인으로 제공한다. 이를 사용하기 위해서는 Snowman 플러그인을 설치해야 한다.
    
- 먼저 실행 중인 x32/x64dbg를 종료한다.
    
- [https://github.com/x64dbg/snowman/releases/tag/plugin-v1](https://github.com/x64dbg/snowman/releases/tag/plugin-v1)
	![[image 1 1.png]]
    
- 위 링크에서 zip파일 Source Code와 snowman.dp32/dp64를 다운받는다.
    
- 다운로드한 zip파일을 압축 해제하여 x32/x64dbg의 로컬 컴퓨터의 `release/x64(또는 x32)/plugins` 폴더에 복사 붙여넣기한다.
    
- 또한, 다운로드한 snowman.dp32/dp64도 각각 x32dbg와 x64dbg의 같은 폴더에 복사 붙여넣기한다.
    
    ![[image 2 1.png]]
    
- x32/x64dbg를 다시 실행하여 코드를 여러 줄 선택하고 마우스 우클릭을 하면 맨 밑에 Snowman 플러그인이 정상적으로 설치된 것을 볼 수 있다.
	![[image 3 1.png]]

### 🔧x64dbg Plugin Manager 설정
https://github.com/horsicq/x64dbg-Plugin-Manager/releases

- 위 사이트에 들어가서 zip파일을 다운로드한다. 
- 다운로드 받은 zip 파일을 압축 해제한 후 압축 해제 된 폴더로 들어간다. 
![[Pasted image 20250812204101.png]]
- 그러면 위와 같이 파일들이 있는데 이 중 x64plgmmr 프로그램을 실행한다. 
- 이후 x64dbg의 루트 디렉터리를 선택하라는 창이 뜨는데, x64dbg 스냅샷 폴더 안의 release 폴더를 선택하고 Enter를 누른다. 
![[Pasted image 20250812204306.png]]

- 위의 사진의 리스트는 모두 Plugn들인데 설치하고 싶은 Plugin의 오른쪽에 있는 `Install`을 눌러 설치하면 된다. 
