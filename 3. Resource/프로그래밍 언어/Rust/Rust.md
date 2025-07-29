### Linked Document

[[첫 번째 Rust 프로그램 만들기]]
[[조건문과 반복문 (Rust)]]
[[소유권 이해하기]]

--- 
### Rust 개발 환경 설정
설치는 `rustup` 이라는 러스트 버전 및 러스트 관련 도구를 관리하는 커맨드라인 도구를 이용할 것이다. 인터넷이 연결되어 있어야 하니 미리 인터넷 연결을 확인해 두자.

####  Rust Installation-Windows
[rustlang_Install Rust](https://www.rust-lang.org/tools/install)

해당 사이트에서 exe 파일을 다운받아 안내에 따라 엔터를 누르면서 실행하면 설치 과정에서 Visual Studio 2013 버전 이상의 MSVC 빌드 도구가 필요하다는 메시지를 출력할 것이다. (단, Visual Studio를 그 전에 설치했다면 나타나지 않을 것이다.)
따라서 빌드 도구를 설치하려면 Visual Studio 2022를 추가로 설치할 필요가 있다. 
이 과정에서 필요한 패키지는 다음과 같다:
* C++ 데스크톱 개발
* Windows 10 또는 11 SDK
* 영어 언어팩과 기타 다른 언어팩

이 과정을 모두 거친 뒤 Rust가 제대로 설치되었는지 확인하는 방법은 Win+R키를 누르고 cmd에 들어가 `rustc --version`을 타이핑하면 최신 릴리즈된 버전 정보가 나타나며, 나타난 정보는 순서대로 버전 숫자, 커밋 해시, 커밋 날짜의 순서대로 나타난다.

#### Visual Studio Code 설정
먼저 VSCode를 설치하고, Win+R 키를 눌러 들어간 cmd에서 `cargo new (프로젝트 이름)`을 입력하고 엔터를 누르면 해당 cmd가 열린 경로에 hello world가 작성된 기본 Rust 프로젝트가 생성된다. 
이후 VSCode를 실행하고 폴더 열기 선택 후 생성된 프로젝트가 있는 폴더를 선택하여 열면 Rust 프로젝트를 VSCode에서 편집할 수 있다. 

그러나 Rust를 정상적으로 구동하기 위해서는 몇 가지의 확장 프로그램을 설치하여야 하는데, 목록과 하는 일은 다음과 같다. 
* rust-analyzer: VSCode에서 Rust를 사용할 수 있도록 코드를 분석해서 보여줌
* CodeLLDB: Rust 디버깅을 위해 사용

컴파일을 하기 위해서는 코드창을 마우스로 눌러 포커싱해주고 Ctrl+Shift+B를 누르면 커맨드라인에 rust:Cargo build가 뜨게 되는데 엔터를 누르면 터미널 창에 에러 없이 컴파일을 진행한다. 
또는 VSCode의 main함수 위에 `Run|Debug`를 클릭하는 버튼이 있고, `Run`을 눌러 실행할 수도 있다.


### Cargo
[카고를 사용해봅시다 - The Rust Programming Language](https://doc.rust-kr.org/ch01-03-hello-cargo.html)
[RUST - Cargo](https://velog.io/@d2h10s/RUST-Cargo)

Cargo는 Rust로 써진 Rust 소스코드 컴파일러인 rustc를 포함하는 빌드 시스템으로, 러스트의 패키지 매니저 역할을 한다. 
즉 Python의 pip나 C/C++의 cmake와 유사한 것이라고 생각하면 된다.
위에서 `Rust 개발 환경 설정 및 실행`부분을 잘 따라 했다면 Cargo가 같이 설치된 상태일 것이다. 

### Rust 학습 (The book)
[The Rust Programming Language - Steve Klablik, Carol Nichols 지음](https://doc.rust-kr.org/title-page.html)
