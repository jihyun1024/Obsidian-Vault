## 👨‍🏫소개

[Rust Playground](https://play.rust-lang.org/?version=stable&mode=debug&edition=2024%EB%8A%94)는 Rust 컴파일러를 브라우저에서 실행할 수 있는 인터페이스로, 로컬로 언어를 설치할 수 없거나 Rust 컴파일러를 사용할 수 없는 경우 플레이그라운드를 사용하여 Rust 코드를 작성하고 실험할 수 있다.

Rust Playground에서 실행되는 모든 코드는 로컬 개발 환경에서도 컴파일하고 실행할 수 있다.

---

## 기본 Rust 프로그램 구조 이해

이 단원에서는 간단한 Rust 프로그램의 구성 방법에 대해 살펴본다.

### Rust의 함수

함수는 특정 작업을 수행하는 하나의 코드 블록으로, 프로그램의 코드를 작업에 따라 블록으로 분리하면 작성한 코드를 파악하고 관리하기 더 수월하다. 함수는 해당 함수를 먼저 정의한 뒤 해당 작업을 수행해야 할 때 함수를 호출해야 정상적으로 함수가 호출된다.

모든 Rust 프로그램에는 이름이 `main`인 함수가 반드시 1개만 있어야 한다. `main`함수는 Rust 프로그램에서 가장 먼저 실행되는 코드이며, `main`함수 또는 다른 함수에서 또 다른 함수를 불러올 수 있다.

```rust
fn main() {
	println!("Hello, world!");
}
```

Rust에서 함수를 선언하기 위해 `fn` 키워드를 사용한다. 함수 이름 다음에는 함수가 전달받는 매개변수 또는 인수의 개수를 컴파일러에 알리며, 인수는 괄호 안에 나열된다.

함수 본문은 함수의 작업을 수행하는 코드이며 중괄호 안에 정의된다. 함수 본문의 여는 중괄호가 괄호 바로 뒤에 있도록 코드의 서식을 지정하는 것이 가장 좋다.

### 코드 들여쓰기

함수 본문에서 대부분의 코드는 세미콜론으로 끝난다. Rust는 세미콜론으로 끝나는 코드를 순서대로 처리하며, 코드 본문이 세미콜론으로 끝나지 않으면 Rust는 그 다음 코드를 시작 코드가 완료되기 전에 실행해야 한다고 인식한다.

코드에서 실행 관계를 쉽게 파악하기 위해 들여쓰기를 사용하며, 이 서식을 보고 코드의 구성과 함수의 작업을 완료하기 위해 필요한 단계의 흐름을 알 수 있다. 시작 코드 문은 왼쪽 여백에서 네 개의 `space`만큼 들여쓰기되며, 코드가 세미콜론으로 끝나지 않으면 실행할 그 다음 코드가 네 개의 `space`만큼 들여쓰기된다.

```rust
fn main() {
	// First step
		// Substep: execute before First step can be completed
		
	// Second step
		// Substep A: execute before Second step can be completed
		// Substep B: execute before Second step can be completed
			// Sub-substep 1: execute before Substep B can be completed
			
	// Third step and so on...
}
```

### Todo! 매크로

todo라는 단어에서 알 수 있듯, 아직 완료되지 않은 코드를 나타내며, Rust 코드를 짤 때 아직 구현되지 않은 코드를 메모해 놓을 때 사용할 수 있다.

예를 들면 다음과 같다.

```rust
fn main() {
	// Display the message "Hello, world!"
	todo!("Display the message by using the println!() macro");
}
```

해당 코드를 실행하면 Rust 컴파일러는 `panic!` 메시지를 반환하며, 이는 심각한 오류가 발생했을 때 프로그램을 터뜨리는 오류 처리방법이며, 배열 범위를 넘어서서 접근하는 등의 심각한 메모리 오류, Result의 값이 오류일 때 unwrap이나 expect를 사용할 때 발생할 수 있지만, `panic!`매크로를 사용하여 디버깅 등 인위적으로 발생시킬 수 있다.

panic을 Python의 try-except 처럼 일반적인 예외 처리 구문으로 사용하는 것은 권장하지 않는다

[https://doc.rust-lang.org/std/panic/fn.catch_unwind.html](https://doc.rust-lang.org/std/panic/fn.catch_unwind.html)

한편, 위의 코드를 실행하면 아래와 같은 메시지가 반환된다.

```rust
Compiling playground v0.0.1 (/playground)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.87s
     Running `target/debug/playground`

thread 'main' panicked at src/main.rs:3:5:
not yet implemented: Display the message by using the println!() macro
note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace
```

### println! 매크로

`println!`매크로는 하나 이상의 입력 인수를 받아 화면이나 표준 출력에 표시한다.

이때, Python처럼 따옴표 종류 상관없이 출력하는 게 아닌 C처럼 큰따옴표만 받아 출력할 수 있다.

```rust
fn main() {
	println!("Hello, world!");
}
```

### {} 인수의 값 대체 (포맷팅)

C언어의 `printf`함수나 Python의 `print`함수처럼,`println!`매크로는 텍스트 문자열 안의 각 중괄호를 목록의 그 다음 인수 값으로 차례차례 바꾼다.

```rust
fn main() {
	println!("The first letter of Alphabet is {} and the last letter is {}.", 'A', 'Z');
}
```

출력은 “The first letter of Alphabet is A and the last letter is Z”이다.

---

## Rust에서 변수 만들기 및 사용

Rust에서는 `let`키워드로 변수를 선언한다. 변수가 선언되면 값을 그 즉시 바인딩(대입)하거나 나중에 프로그램 진행 중 해당 변수가 바인딩될 수 있다.

```rust
let a_number;
// 아직 값에 바인딩되어 있지 않음

let a_number = 23;
// 변수에 값을 바인딩
```

<aside> 💡

다른 프로그래밍 언어처럼 `fn`이나 `let`같은 특정 키워드는 Rust에서만 사용하도록 예약된 단어로, 함수나 변수의 이름으로 사용할 수 없다.

</aside>

다른 예로, 다음 코드는 두 개의 변수를 선언하고 `println!`매크로를 호출하여 두 변수의 값을 표시한다.

```rust
let a_number;
let a_word = "Ten";

a_number = 10;

println!("The number is {}.", a_number);
println!("The word is {}.", a_word);
```

`println!`매크로를 호출하여 바인딩 되기 전에 `a_number`변수 값을 표시하려 하면 컴파일러가 오류를 반환하며, 이 오류 메시지는 [https://play.rust-lang.org/?version=stable&mode=debug&edition=2024에서](https://play.rust-lang.org/?version=stable&mode=debug&edition=2024%EC%97%90%EC%84%9C) 직접 확인할 수 있다.

### 변경 불가 및 변경 가능

Rust에서는 기본적으로 변수 바인딩을 변경할 수 없다. 예를 들어 전 예제의 `a_number`값을 Python이나 C/C++과 비슷한 방식으로 그냥 변경하려 하면 컴파일러에서 오류 메시지를 표시한다.

따라서 변수의 값을 변경하려면 애초에 변수를 선언할 때 `mut`키워드를 사용해 변수의 값을 변경할 수 있게 해야 한다.

```rust
// The `mut` keyword lets the variable be changed
let mut a_number = 10; 
println!("The number is {}.", a_number);

// Change the value of an immutable variable
a_number = 15;
println!("Now the number is {}.", a_number);

// Output
// The number is 10.
// Now the number is 15.
```

변수 `a_number`를 변경할 수 있기 때문에 이 코드는 문제 없이 컴파일 되는 것을 확인할 수 있다.

### 변수 섀도잉

섀도잉은 기존 변수의 이름을 사용하는 새로운 변수를 선언하여 새로운 바인딩을 생성하는 것을 말하며, 이전 변수는 여전히 존재하지만, 섀도잉된 범위에서는 더 이상 참조할 수 없다.

다음 코드는 섀도잉 사용을 보여준다.

```rust
fn main() {
	let shadow_num = 5;
	let shadow_num = shadow_num + 5;
	let shadow_num = shadow_num * 2;
	
	println!("The number is {}.", shadow_num);
}
```

해당 코드의 출력은 잘 알 수 있을 것이라 생각한다.

---

## 숫자, 텍스트 및 true/false 값에 대한 데이터 형식

Rust는 정적 언어로, Rust 컴파일러는 프로그램이 컴파일하고 실행할 코드에 있는 모든 변수의 정확한 데이터 타입을 알아야 한다. Rust 프로그램을 작성할 때는 컴파일러가 일반적으로 바인딩된 값에 따라 데이터 타입을 자동으로 유추할 수 있어 코드에 데이터 타입을 항상 명시할 필요는 없지만, 여러 데이터 타입을 사용하는 경우 컴파일러에 특정 데이터 타입을 알려야 한다.

다음 예시에서는 `number`변수를 32비트 정수로 만들도록 컴파일러에 지시한다.

데이터 타입을 지정하는 방법은 변수 이름 뒤에 콜론을 사용하여 지정할 수 있다.

```rust
let number: u32 = 14;
println!("The number is {}.", number);
```

`number`변수의 값을 큰따옴표로 묶으면 컴파일러가 값을 숫자가 아닌 텍스트로 해석한다. 이 때는 컴파일러가 해석한 변수 값의 데이터 타입이 코드에 지정된 `u32`데이터 타입과 일치하지 않으므로 컴파일러가 오류를 발생시킨다.

### 기본 제공 데이터 타입

Rust는 기본적으로 숫자, Bool, 텍스트 등 몇 가지 데이터 타입을 기본적으로 제공한다.

기본적으로 제공하는 데이터 타입은 다음과 같다.

- 정수
- 부동 소수점 숫자
- Bool
- 문자 및 문자열
- 튜플 등 데이터 계열의 더 복잡한 데이터 타입

### 정수 및 부동 소수점

Rust의 정수는 비트 크기와 부호 유무로 식별되며, 부호 있는 정수는 양수 또는 음수이고, 부호 없는 정수는 양수만 될 수 있다.

|길이|부호 있는 정수|부호 없는 정수|
|---|---|---|
|8bit|i8|u8|
|16bit|i16|u16|
|32bit|i32|u32|
|64bit|i64|u64|
|128bit|i128|u128|
|아키텍처 종속|isize|usize|

아키텍처 종속 타입은 컴퓨터의 종류에 따라 달라지며, 64비트 타입은 64비트 아키텍처에서 사용되고 32비트 타입은 32비트 아키텍처에서 사용된다.

정수에 대한 형식을 따로 지정하지 않고 시스템에서 형식을 유추할 수 없는 경우 기본적으로 `i32`타입이 할당된다.

Rust에는 10진수 값 `f32`(32비트)와 `f64`(64비트)의 두 가지 부동 소수점 데이터 타입이 존재한다. 기본 부동 소수점 데이터 타입은 `f64` 이며, 최신 CPU에서 `f64`타입은 `f32`타입과 속도는 거의 같지만 더 정밀하다.

또한, Rust의 모든 기본 숫자 타입은 사칙연산을 전부 지원한다.

### True 또는 False

Bool 형식으로 진위 여부를 저장하며, 조건식에서 값이 true이면 작업을 수행하고, 값이 false이면 다른 작업을 수행하는 방식으로 많이 사용한다.

```rust
let is_bigger = 1 > 4;
println!("Is 1 > 4? {}", is_bigger);

// Output
// Is 1 > 4? false
```

### 텍스트: 문자 및 문자열

모든 텍스트 형식은 UTF-8로 표현되며, 문자가 단일 항목이라면 문자열은 문자들을 모아 놓은 배열과 같다.

`char`형식은 항목을 작은 따옴표로 묶어 지정한다.

```rust
let uppercase_s: char = 'S';
let lowercase_f: char = 'f';
let smiley_face: char = '😃';
```

`str`형식은 문자열 데이터이며, 대부분의 경우 `&str`이 있는 형식 앞의 참조 스타일 구문을 사용하여 지정하고, 큰따옴표를 사용한다.

<aside> 💡

런타임 동안 사용자가 프로그램과 상호작용하고 터미널을 통해 텍스트를 받는 경우, Rust에는 `String`이라는 두 번째 문자열 형식이 있다. 이 형식은 힙 영역에 할당되며, 이 타입을 사용하면 코드가 컴파일되기 전에 문자열의 길이를 알 필요가 없다.

이 두 타입의 차이를 알려면 Rust의 소유권 및 대여 시스템에 대해 알아야 하므로 그때까지는 `String`타입 데이터를 프로그램이 실행될 때 변경될 수 있는 텍스트 데이터로, `&str`타입 데이터를 변하지 않는 텍스트 데이터로 생각할 수 있다.

</aside>

### 텍스트 예시

```rust
fn main() {
	let uppercase_s: char = 'S';
	let lowercase_f: char = 'f';
	let smiley_face: char = '😃';
	
	let str1: &str = "miley";
	let str2: &str = "ace";
	
	println!("{} is {}{}_{}{}", smiley_face, uppercase_s, str1, lowercase_f, str2);
}

// Output
// 😃 is Smiley_face
```

---

## 튜플 및 구조체

### 튜플(tuple)

Python 처럼 튜플은 하나의 데이터로 수집되는 다양한 데이터 타입의 값들을 그룹화한 것으로, 튜플의 개별적인 값들을 튜플의 요소라고 하며, 요소들은 쉼표로 구분되고 괄호로 묶어서 튜플로 지정한다.

튜플에는 요소의 수와 동일한 고정 길이가 있으며, 튜플이 한 번 선언되면 요소를 더 추가하거나 제거할 수 없다.

튜플의 요소에 액세스할 때는 0부터 시작하는 인덱스 위치에서 액세스할 수 있으며, 액세스할 때는 구문 `<tuple>.<index>`를 사용한다.

```rust
fn main() {
	// Declare a tuple of three elements
	let tpl: (char, i32, bool) = ('E', 5i32, true);
	
	// Use tuple indexing and show the values of the elements in the tuple
	println!("Is '{}' the {}th letter of the alphabet? {}", tpl.0, tpl.1, tpl.2);
}

// Output
// Is 'E' the 5th letter of the alphabet? true
```

튜플은 여러 데이터 타입을 단일 값으로 결합해서 사용하려는 경우에 유용하게 사용할 수 있으며, 또한 많은 값들을 포함할 수 있으므로 함수에서 튜플을 사용하여 여러 값들을 반환할 수 있다.

### 구조체 (클래식 구조체 vs 튜플 구조체 vs 단위 구조체)

구조체는 다른 데이터 타입들로 구성된 하나의 타입으로, 구조체의 각각의 요소를 필드라고 하며, 튜플처럼 구조체의 필드에도 서로 다른 데이터 타입들이 있을 수 있다.

Rust 프로그램에서 구조체를 사용하려면 먼저 이름으로 구조체를 정의하고 각 필드에 대한 데이터 타입을 지정해야 한다.

Rust는 클래식 구조체, 튜플 구조체 및 단위 구조체를 지원하며, 각각은 다음과 같다.

- 클래식 C 구조체: 가장 일반적으로 사용되는 구조체로, 각 필드에는 이름과 데이터 타입이 명시되어 있다. 클래식 구조체를 정의한 후에는 `<struct>.<field>`구문을 사용하여 구조체의 필드에 액세스할 수 있다.
- 튜플 구조체: 클래식 구조체와 유사하지만, 필드에 이름이 없는 구조체로, 튜플 구조체의 필드에 액세스하려면 튜플처럼 `<struct>.<index>`구문을 사용할 수 있다
- 단위 구조체: 표식으로 가장 일반적으로 사용되며, Rust의 특성 기능에 대해 알아볼 때 더 자세히 알 수 있다.

```rust
// Classic C struct
struct Student { name: String, level: u32, remote: bool }

// Tuple struct 
struct Grades(char, char, char, char, f32);

// Unit struct
struct Unit;
```

구조체를 정의하려면 구조체 이름 앞에 `struct`키워드를 입력하며, 구조체 타입의 이름은 대문자로 시작해야 한다.

구조체 타입은 Rust 프로그램에서 main함수 및 기타 함수 밖에 정의되기도 하며, 따라서 구조체 정의의 시작은 왼쪽 여백에서 들여쓰기되지 않고 정의의 내부 부분만 들여쓰기된다.

- 클래식 구조체
    
    함수처럼 클래식 구조체의 본문은 중괄호 안에 정의된다. 클래식 구조체의 각 필드에는 구조체 내에서 고유한 이름이 지정되며, 각 필드의 데이터 타입은 `:<type>`을 통해 지정된다. 클래식 구조체의 필드는 쉼표로 구분되며, **중괄호까지 다 쓰면 세미콜론으로 끝나지 않는다.**
    
    클래식 구조체의 장점은 이름으로 구조체 필드의 값에 액세스할 수 있다.
    
- 튜플 구조체
    
    튜플처럼 튜플 구조체의 본문도 중괄호가 아닌 괄호 안에 정의된다. 괄호는 구조체 이름 바로 뒤에 띄어쓰기 없이 바로 오며, 튜플과 달리 튜플 구조체 정의에는 각 필드의 데이터 타입만 포함된다.
    

### 구조체 인스턴스화

구조체를 정의한 후에 구조체 타입의 인스턴스를 만들고 각 필드의 값을 함수에서 지정하여 구조체를 사용할 수 있다.

필드의 값을 지정할 때는 구조체를 정의한 순서와 동일한 순서로 필드의 값을 지정하지 않아도 된다.

```rust
struct Student {name: String, remote: bool, level: i32}
struct Grades(char, char, char, char, f32);

fn main() {
    // Instantiate classic struct, specify fields in random order, or in specified order
    let user_1 = Student { name: String::from("Constance Sharma"), remote: true, level: 2 };
    let user_2 = Student { name: String::from("Dyson Tan"), level: 5, remote: false };

    // Instantiate tuple structs, pass values in same order as types defined
    let mark_1 = Grades('A', 'A', 'B', 'A', 3.75);
    let mark_2 = Grades('B', 'A', 'A', 'C', 3.25);

    println!("{}, level {}. Remote: {}. Grades: {}, {}, {}, {}. Average: {}", 
         user_1.name, user_1.level, user_1.remote, mark_1.0, mark_1.1, mark_1.2, mark_1.3, mark_1.4);
    println!("{}, level {}. Remote: {}. Grades: {}, {}, {}, {}. Average: {}", 
         user_2.name, user_2.level, user_2.remote, mark_2.0, mark_2.1, mark_2.2, mark_2.3, mark_2.4);
}

// Ouptut
// Constance Sharma, level 2. Remote: true. Grades: A, A, B, A. Average: 3.75
// Dyson Tan, level 5. Remote: false. Grades: B, A, A, C. Average: 3.25
```

### 문자열 리터럴을 문자열 형식으로 변환

구조체 또는 벡터와 같은 다른 데이터 타입 내에 저장된 문자열 데이터는 문자열 리터럴 참조(`&str`)에서 `String`형식으로 변환되어야 한다.

이때 해당 변환을 수행하려면 `String::from(&str)`메서드를 사용한다. 만약 값을 할당하기 전에 형식을 변환하지 않으면 컴파일러에서 오류를 발생시킨다.

```rust
error[E0308]: mismatched types
  --> src/main.rs:24:15
   |
24 |         name: "Dyson Tan",
   |               ^^^^^^^^^^^
   |               |
   |               expected struct `String`, found `&str`
   |               help: try using a conversion method: `"Dyson Tan".to_string()`

error: aborting due to previous error
```

해당 결과에서는 `.to_string()`함수를 이용해서 변환할 수 있다고 제안하지만, 이 예제에서는 `String::from(&str)`메서드를 사용하여 형식을 변환한다.

---

## 열거형 변형 사용

Rust에서 열거형은 일반적으로 상태를 정의하기 위한 타입으로, 하나의 타입에 여러 선택 가능한 값을 정의해두고 이 값들 중 하나를 선택할 수 있게 할 수 있는 특수한 타입이다.

`enum`키워드를 사용하여 열거형 형식을 만들며, 구조체처럼 열거형도 대문자로 시작한다.

### 열거형 정의

웹 이벤트를 분류하는 열거형을 정의한다.

```rust
enum WebEvent {
    // An enum variant can be like a unit struct without fields or data types
    WELoad,
    // An enum variant can be like a tuple struct with data types but no named fields
    WEKeys(String, char),
    // An enum variant can be like a classic struct with named fields and their data type
    WEClick { x: i64, y: i64 }
}
```

- `WELoad`는 연결된 데이터 타입이 없다.
- `WEKeys`는 문자열, 문자 데이터 타입이 있는 필드가 두 개 있다.
- `WEClick`은 이름이 있는 필드 x, y가 있는 구조체와 그 데이터 타입인 `i64`가 있다.

이 열거형의 모든 변형들은 동일한 `WebEvent`열거형 형식으로 그룹화되며, `WebEvent`열거형의 변형을 사용하는 함수는 열거형의 모든 변형들을 허용해야 한다.

열거형의 변형이란 열거형 내에서 묶어진 값들을 말하며, 예를 들어 각 교통수단의 세부 정보를 포함하는 열거형은 다음과 같이 나타낼 수 있다.

```rust
enum Transportation {
	Car(String, u32), // 모델명, 속도
	Bike(String), // 모델명
	Train(u32), // 속도
	Airplain { company: String, capacity: u32 }, // 운용 회사, 수용 인원
}
```

### 구조체를 사용한 열거형 정의

구조체를 사용해서 웹 이벤트를 분류하는 열거형을 정의할 수 있다. 그러면 열거형의 각각의 변형이 해당 구조체를 사용하며 이 스타일의 정의를 통해 각각의 논리적 변형을 참조할 수 있다.

```rust
struct KeyPress(String, char);
struct MouseClick { x: i64, y: i64 }

enum WebEvent { WELoad(bool), WEClick(MouseClick), WEKeys(KeyPress) }
```

### 열거형 인스턴스화

열거형의 특정 변형에 액세스하려면 이중 콜론을 사용하여 `<enum>::<variant>`를 사용한다.

이 단원에서는 예제 코드를 이용하여 설명한다.

먼저 단순 변형의 경우, `WebEvent`열거형의 첫 번째 변형에는 단일 bool값 `WELoad(bool)`이 있다. 앞 단원에서 bool을 사용했던 방법과 비슷한 방법으로 이 변형을 인스턴스화한다.

```rust
struct KeyPress(String, char);
struct MouseClick { x: i64, y: i64 }
enum WebEvent { WELoad(bool), WEClick(MouseClick), WEKeys(KeyPress) }

let load = WebEvent::WELoad(true);
```

구조체 변형의 경우 클래식 구조체인 `WEClick(MouseClick)` 이 포함되어 있다. 이 구조체는 이름 있는 필드 x와 y가 있으며 두 필드 모두 `i64`데이터 타입을 가진다.

이 변형을 만드려면 먼저 해당 구조체를 인스턴스화 한 뒤 호출에서 구조체를 인수로 전달한다.

```rust
struct KeyPress(String, char);
struct MouseClick { x: i64, y: i64 }
enum WebEvent { WELoad(bool), WEClick(MouseClick), WEKeys(KeyPress) }

let click = MouseClick { x: 100, y: 100 };
let we_click = WebEvent::WEClick(click);
```

마지막 변형인 튜플 변형의 경우 튜플 `WEKeys(KeyPress)`가 있다. 이 튜플은 문자열과 문자 하나를 사용하는 필드 두 개가 있다.

이 변형을 만드려면 먼저 튜플을 인스턴스화 한 뒤 호출에서 튜플을 인수로 전달한다.

```rust
struct KeyPress(String, char);
struct MouseClick { x: i64, y: i64 }
enum WebEvent { WELoad(bool), WEClick(MouseClick), WEKeys(KeyPress) }

let keys = KeyPress(String::from("Ctrl+"), 'N');\\
let we_keys = WebEvent::WEKeys(keys);
```

이 코드에서는 문자열을 전달하기 위해 `String::from("<value>")`구문을 사용하였다. 이 구문은 Rust의 `from`메서드를 호출하여 `String`값을 만들며, 큰따옴표로 묶인 문자열을 인수로 받는다.

### 열거형 예제 코드

다음은 열거형 변형들을 인스턴스화한 최종 코드이다.

```rust
#[derive(Debug)]
struct KeyPress(String, char);

#[derive(Debug)]
struct MouseClick { x: i64, y: i64 }

#[derive(Debug)]
enum WebEvent { WELoad(bool), WEClick(MouseClick), WEKeys(KeyPress) }

fn main() {
    let click: MouseClick = MouseClick { x: 100, y: 200 };
    println!("Mouse click location: {}, {}", click.x, click.y);

    let keys: KeyPress = KeyPress(String::from("Ctrl+"), 'N');
    println!("\\nKeys pressed: {}{}", keys.0, keys.1);

    let we_load = WebEvent::WELoad(true);
    let we_click = WebEvent::WEClick(click);
    let we_keys: WebEvent = WebEvent::WEKeys(keys);

    println!("\\nWebEvent enum structures: \\n\\n{:#?} \\n\\n {:#?} \\n\\n {:#?}", we_load, we_click, we_keys);
}
```

`#[derive(Debug)]`문을 사용하면 코드 실행 시 표준 출력에서 볼 수 없는 특정 값들을 볼 수 있다.

여기서는 `println!`매크로를 사용하여 디버그 데이터를 볼 수 있도록 `{:#?}`구문을 사용하여 읽을 수 있는 방식으로 데이터의 형식을 지정한다.

즉, `#[derive(Debug)]`문은 코드를 디버깅 모드로 실행하는 코드로 볼 수 있다.

---

## Rust의 함수

Rust에서 함수를 어떻게 정의하고 사용하는가 알아볼 것이다.

### 함수 정의

Rust의 함수 정의는 `fn`으로 시작하며, 함수 이름 뒤에 함수의 입력 인수를 괄호 안에 넣고, 함수 본문을 여는 중괄호는 일반적으로 괄호 뒤에 배치한다.

Rust에서는 파일 어딘가에 함수가 정의되어 있기만 하면 된다.

```rust
fn main() {
	println!("Hello, world!");
	goodbye();
}

fn goodbye() {
	println!("\\nGoodbye");
}
```

함수에 입력 인수가 없으면 괄호를 비워 둔다.

### 입력 인수 전달

함수에 입력이 있는 경우 각 인수의 이름을 지정하고 함수 선언 시점에 데이터 형식을 지정해야 한다. 인수는 변수처럼 이름이 지정되므로 함수 본문의 인수에 접근할 수 있게 된다.

```rust
fn goodbye(message: &str) {
	println!("\\n{}", message);
}

fn main() {
	let formal = "Formal: Goodbye";
	let casual = "Casual: See you later!";
	goodbye(formal);
	goodbye(casual);
}
```

### 값 반환

함수를 실행한 후 값을 반환해야 하는 경우, 그에 맞는 데이터 타입을 함수 인수 목록의 닫는 괄호와 함수 본문을 여는 중괄호 사이에 `-> <type>`을 추가한다.

이는 함수가 호출자에게 값을 반환한다는 것을 의미하며, `<type>`을 통해 컴파일러는 반환값의 데이터 타입을 알 수 있다.

```rust
fn divide(num: u32) -> u32 {
	num / 5
}

fn main() {
	let num = 25;
	println!("{} divided by 5 = {}", num, divide(num));
}

// Output
// 25 divided by 5 = 5
```

가끔 오류 검출이나 디버깅 등의 이유로 함수를 중간에 끝내려는 경우, 함수의 임의의 지점에서 `return`을 사용하여 실행을 중단하고 호출자에게 값을 다시 보낼 수 있다.

`return`을 명시적으로 사용할 때는 세미콜론을 이용하며, `return`을 사용하지 않고 반환 값을 다시 보낼 때는 세미콜론을 이용하지 않는다. (위의 `divide`함수에서도 세미콜론을 쓰지 않았다)

```rust
fn divide(num: u32) -> u32 {
	if num == 0 {
		return 0;
	}
	num / 5;
}
```

---

## 🧑‍💻연습: 자동차를 빌드하는 함수 작성

도전 과제: 샘플 코드를 컴파일하고 실행되도록 수정하는 것 → 새 자동차 주문을 처리

### 열거형 사용

```rust
// 고쳐야 할 코드
// Declare Car struct to describe vehicle with four named fields
struct Car {
    color: String,
    transmission: Transmission,
    convertible: bool,
    mileage: u32,
}

#[derive(PartialEq, Debug)]
// Declare enum for Car transmission type
enum Transmission {
    // todo!("Fix enum definition so code compiles");
    Manual;
    SemiAuto;
    Automatic;
}
```

Transmission 열거형에서 Manual, SemiAuto, Automatic을 정의하는 과정에서 콤마 대신 세미콜론을 써서 오류가 발생함 → 세미콜론 대신 콤마 사용

### 구조체 사용

`car_factory`함수에 대한 코드를 추가하여 `Car`구조체의 인스턴스를 만든다. (입력 인수의 값을 사용하여 자동차의 특성을 할당, 각각 색깔, 변속기 속성, 컨버터블 여부, 주행 거리)

```rust
// 고쳐야 할 코드
// Build a "Car" by using values from the input arguments
// - Color of car (String)
// - Transmission type (enum value)
// - Convertible (boolean, true if car is a convertible)
fn car_factory(color: String, transmission: Transmission, convertible: bool) {

    // Use the values of the input arguments
    // All new cars always have zero mileage
    let car: Car = todo!("Create an instance of a `Car` struct");
}
```

방금 전 코드에서 `Car`구조체에는 color, transmission, convertible, mileage 속성이 있었고 모든 새 자동차의 주행 거리는 0이 되어야 하므로 `todo`를 아래와 같이 변경해야 한다.

```rust
fn car_factory(color: String, transmission: Transmission, convertible: bool) {
	// 함수의 인자로 들어오는 것들을 mileage만 뺴고 다 그대로 받게 설정
	let car: Car = Car { color: color, transmission: transmission, convertible: convertible, mileage: 0 };
}
```

### 함수에서 값 반환 사용

이제 아까 만든 `Car`구조체를 함수에서 반환하도록 `car_factory`함수를 업데이트해야 한다. 값을 반환하려면 함수 본문에 값이 있어야 하고, 함수에서 반환값의 타입을 선언해야 한다.

또한, 방금 전 `let car: Car = Car {...}`문을 변경할 때 중복되는 코드가 있어 번거로웠는데, 이 단계를 완료하면 구조체 인스턴스화에서 중괄호 안의 세미콜론들을 지울 수 있다.

따라서 최종 정리본은 다음과 같다.

```rust
fn car_factory(color: String, transmission: Transmission, convertible: bool) -> Car {
	// mileage: 는 지우면 어떤 속성을 0으로 설정하는 건지 모르기 때문에 건드리면 안 됨
	let car: Car = Car {color, transmission, convertible, mileage: 0};
	car
}
```

### 함수 호출

이제 공들여 만든 함수를 호출하고 일부 자동차를 빌드할 준비가 되었다.

`main`함수를 기존 코드에 추가해 주고 코드를 다시 빌드하면 제대로 코드를 짰다는 가정 하에 정상적으로 동작할 것이다.

```rust
// Declare Car struct to describe vehicle with four named fields
struct Car {
    color: String,
    transmission: Transmission, 
    convertible: bool,
    mileage: u32,
}

#[derive(PartialEq, Debug)]
// Declare enum for Car transmission type
enum Transmission {
    Manual, 
    SemiAuto,
    Automatic,
}

// Build a "Car" by using values from the input arguments
// - Color of car (String)
// - Transmission type (enum value)
// - Convertible (boolean, true if car is a convertible)
fn car_factory(color: String, transmission: Transmission, convertible: bool) -> Car {
    let car: Car = Car { color, transmission, convertible, mileage: 0 };
    car 
    // car 인스턴스를 구조체의 형태로 반환해야 하므로 return과 세미콜론없이 car만 표기
}

fn main() {
		// We have orders for three new cars!
    // We'll declare a mutable car variable and reuse it for all the cars
		
    let mut car = car_factory(String::from("Red"), Transmission::Manual, false);
    println!("Car 1 = {}, {:?} transmission, convertible: {}, mileage: {}", car.color, car.transmission, car.convertible, car.mileage);
    
    car = car_factory(String::from("Silver"), Transmission::Automatic, true);
    println!("Car 2 = {}, {:?} transmission, convertible: {}, mileage: {}", car.color, car.transmission, car.convertible, car.mileage);

    car = car_factory(String::from("Yellow"), Transmission::SemiAuto, false);
    println!("Car 3 = {}, {:?} transmission, convertible: {}, mileage: {}", car.color, car.transmission, car.convertible, car.mileage);    
}

// Output
// Car 1 = Red, Manual transmission, convertible: false, mileage: 0
// Car 2 = Silver, Automatic transmission, convertible: true, mileage: 0
// Car 3 = Yellow, SemiAuto transmission, convertible: false, mileage: 0
```

---

## 요약

이 모듈에서는 Rust 프로그램의 기본 구조를 알아보았다. `main`함수는 Rust 프로그램의 진입점이다. `println!`매크로를 사용하여 변수 값을 표시하고 프로그램의 진행 상황을 알 수 있으며, `let`키워드로 변수를 정의하고, `mut`키워드로 해당 값을 변경 가능하게 할 수 있다.

Rust 언어는 문자열, 숫자, bool, 부동 소수점 등 데이터 타입을 엄격하게 해석하는 언어로, 데이터 타입이 올바르게 정의되고 사용되는 경우에만 정상적으로 동작한다.