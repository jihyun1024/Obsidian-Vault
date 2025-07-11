어떤 조건이 참인지에 따라 특정 코드를 실행하고, 어떤 조건을 만족하는 동안 특정 코드를 반복 수행하는 조건문과 반복문은 대부분의 프로그래밍 언어에서 매우 중요한 문법이다.

Rust에서는 if 표현식과 반복문이 많이 사용된다.

## `if` 표현식

`if`표현식은 코드가 조건에 따라 분기할 수 있도록 해 준다. 조건을 제공한 뒤 조건을 만족하면 해당 코드 블록을 실행하고, 그렇지 않다면 코드 블록을 실행하지 않는 식으로 동작한다.

```rust
fn main() { 
	let number = 3;
	
	if number < 5 {
		println!("condition is true");
	} else {
		println!("condition is false");
	}
}

// Output
// condition is true
```

모든 `if`표현식은 `if` 라는 키워드로 시작하고 그 뒤에 조건이 온다. 조건이 참일 때 실행하는 코드 블록은 조건 바로 뒤 중괄호로 된 블록에 배치되며, 위의 코드처럼 `else`표현식을 붙일 수도 있는데, 이는 해당 조건이 거짓일 경우 실행되는 코드 블록을 제공한다.

`else`표현식이 없는 상태에서 조건이 거짓이라면 프로그램은 `if`블록을 생략하고 다음 코드로 넘어간다.

또한, 조건식은 반드시 bool이어야 한다는 점을 주목해야 한다. 조건식이 bool이 아니면 에러가 발생한다.

```rust
fn main() {
	let number = 3;
	
	if number {
		println!("condition is true");
	} else {
		println!("condition is false");
	}
}

// Output
error[E0308]: mismatched types
  --> src\\main.rs:4:5
  |
4 |     if number {
  |        ^^^^^^ expected `bool`, found integer

For more information about this error, try `rustc --explain E0308`.
error: could not compile `test_rust` (bin "test_rust") due to 1 previous error
```

이 코드에서는 if 조건식의 결과가 3이고, Rust 컴파일러는 에러를 발생시킨다.

이 에러의 뜻은 Rust가 bool을 조건식의 결과로 예상했으나, 정수값을 받았다는 의미이며, Ruby나 JavaScript 등의 언어와 달리 Rust는 bool 타입이 아닌 값을 bool 타입으로 자동 변환하지 않는다.

따라서 if문에는 항상 명시적으로 bool 타입의 조건식을 제공해야 한다.

### `else if` 로 여러 조건식 다루기

`if`조건식은 단순히 `if`와 `else`만 사용하는 것이 아니라 C처럼 중간에 `else if`라는 표현식을 집어 넣어 분기를 더 늘릴 수 있다. 예를 들면

```rust
fn main() { 
	let number = 6;
	
	if number % 4 == 0 {
		println!("number is divisible by 4");
	} else if number % 3 == 0 {
		println!("number is divisible by 3");
	} else if number % 2 == 0 {
		println!("number is divisible by 2");
	} else {
		println!("number is not divisible by 4, 3, or 2");
	}
}
```

이 프로그램은 분기 가능한 4개의 경로가 있다. 실행하면 어떻게 될지 추측하라.

Rust는 처음으로 true가 되는 조건의 코드 블록을 실행하고 나면 다른 나머지 조건은 검사도 하지 않고 바로 조건문을 빠져 나와 다른 블록의 텍스트는 출력되지 않는다.

`else if`표현식을 너무 많이 사용하면 코드가 복잡해질 수 있기 때문에 표현식이 2개 이상이면 코드를 리팩터링하는 것이 좋다. 나중에 이런 경우에 적합한 `match`라는 Rust의 C에서의 `switch`와 비슷한 강력한 분기 구조에 대해 알아볼 것이다.

### `let` 구문에서 `if` 사용하기

`if`자체는 표현식이기 때문에 변수에 값을 바로 바인딩하기 위해 `let`구문의 오른쪽에 사용가능하다. 즉, `if-else`문은 값을 가질 수 있다.

```rust
fn main() {
	let condition = true;
	let number = if condition { 5 } else { 6 };
	
	println!("The value of number is: {}", number);
}

// Output
// The value of number is: 5
```

코드 블록은 블록 안의 표현식을 계산하고, 숫자는 그 자체로 표현식이 되며, condition 변수의 값이 true이기 때문에 number 변수에는 i32 정수 5가 바인딩된다.

```rust
fn main() {
	let condition = true;
	let number = if condition { 5 } else { "six" };
	
	println!("The value of number is: {}", number);
}

// Output
error[E0308]: `if` and `else` have incompatible types
 --> src/main.rs:4:44
  |
4 |     let number = if condition { 5 } else { "six" };
  |                                 -          ^^^^^ expected integer, found `&str`
  |                                 |
  |                                 expected because of this

For more information about this error, try `rustc --explain E0308`.
error: could not compile `branches` due to previous error
```

변수가 가질 수 있는 타입은 오직 하나이고, 따라서 컴파일러는 컴파일 시점에 number 변수의 타입이 무엇인지 알 필요가 있다. Rust에서는 변수의 타입이 컴파일 시점이 아닌 런타임 시점에 정의되도록 할 수 없다.

따라서 이 경우는 `if`의 분기와 `else`의 분기의 값이 한 쪽은 i32이고 한 쪽은 문자열이라 분기 값의 타입이 호환되지 않기 때문에 이 코드를 동작 시키면 오류가 발생한다.

## 반복문을 이용한 반복

반복적인 작업을 위해 Rust는 몇 가지 반복문을 제공하는데 이는 반복문 본문의 시작에서 끝까지 수행한 뒤 다시 처음부터 수행한다.

Rust에는 `loop`, `while`, 그리고 `for`세 가지 종류의 반복문이 있다. 하나씩 사용해 보자.

### `loop` 로 코드 반복하기

`loop` 키워드는 코드를 그만두라고 명시적으로 알려주기 전까지, 또는 영원히 코드 블록을 반복적으로 수행하도록 한다.

```rust
fn main() {
	loop {
		println!("again!");
	}
}
```

이 프로그램을 실행하면 Ctrl+C를 눌러 프로그램을 강제로 정지시키기 전까지 `again!`을 계속 출력하는 것을 확인할 수 있다.

기호 `^C` 는 우리가 Ctrl+C를 누른 지점을 표시한다. 코드가 정지 신호를 받은 시점에 따라 이 기호 이후에 `again!`이 출력될 수도 있고, 출력되지 않을 수도 있다.

루프 안에 `break`키워드를 집어넣으면 `loop`를 멈춰야 하는 시점을 프로그램에 알려줄 수 있다.

```rust
use rand::Rng;
use std::cmp::Ordering;
use std::io;

fn main() {
    println!("Guess the number!");

    let secret_number = rand::thread_rng().gen_range(1..=100);

    loop {
        println!("Please input your guess.");

        let mut guess = String::new();

        io::stdin()
            .read_line(&mut guess)
            .expect("Failed to read line");

        let guess: u32 = match guess.trim().parse() {
            Ok(num) => num,
            Err(_) => continue,
        };

        println!("You guessed: {guess}");

        match guess.cmp(&secret_number) {
            Ordering::Less => println!("Too small!"),
            Ordering::Greater => println!("Too big!"),
            Ordering::Equal => {
                println!("You win!");
                break;
            }
        }
    }
}
```

이 프로그램의 경우 `secret_number`를 받아 같다면 “You win!”을 출력하고 `break`로 빠져나온다.

### 반복문에서 값 반환하기

`loop`의 사용법 중 하나는 어떤 스레드가 실행 완료되었는지 검사하는 등 실패할 수도 있는 작업을 재시도할 때 사용할 수 있다.

여기서 해당 연산의 결과를 이후의 코드에 전달하고 싶을 때는 루프 정지를 위해 사용한 `break`뒤에 반환하고자 하는 값을 넣으면 된다.

해당 값은 아래와 같이 반복문 밖으로 반환되어 사용 가능하게 된다.

```rust
fn main() {
	let mut counter = 0;
	
	let result = loop {
		counter += 1;
		if counter == 10 {
			break counter * 2;
		}
	};
	
	println!("The result is {}", result);
}
```

이 코드에서는 반복문 전에 `counter`라는 변수를 선언하여 0으로 초기화하고, `result`라는 변수를 선언하여 `loop`반복문으로부터 반환된 값을 저장하도록 하였다. 반복문의 매 회차마다 `counter`변수에 1을 더한 후 값이 10과 같은지 검사한다.

`counter`의 값이 10과 같은 경우 `break`키워드와 `counter * 2`를 사용하여 값을 반환하며 반복문을 나가게 하였다.

루프 뒤에는 `result`에 값을 할당하는 구문을 끝내기 위해 닫는 중괄호 뒤에 세미콜론을 붙였다. 결과적으로 `result`의 값은 20이 되어 “The result is 20”이 출력된다.

### 루프 라벨로 여러 반복문 사이의 모호함 없애기

만약 `loop`가 여러 번 사용된 경우 `break`와 `continue`는 해당 지점의 바로 바깥쪽 `loop`에 적용한다. 루프에 루프 라벨(loop label)을 추가적으로 명시하면 `break`와 `continue`와 함께 이 키워드들이 바로 바깥쪽 루프 대신 라벨이 적힌 특정한 루프에 적용되도록 할 수 있다.

루프 라벨은 반드시 작은 따옴표로 시작해야 한다.

```rust
fn main() {
	let mut count = 0;
	'counting_up: loop {
		println!("count = {}", count);
		let mut remaining = 10;
		
		loop {
			println!("remaining = {}", remaining);
			if remaining == 9 {
				break;
			}
			if count == 2 {
				break 'counting_up;
			}
			remaining -= 1;
		}
		count += 1;
	}
	println!("End count = {}", count);
}

// Output
//   Compiling test_rust v0.1.0 (C:\\Users\\rkdwl\\test_rust)
//    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.47s
//     Running `target\\debug\\test_rust.exe`
// count = 0
// remaining = 10
// remaining = 9
// count = 1
// remaining = 10
// remaining = 9
// count = 2
// remaining = 10
// End count = 2
```

바깥쪽 루프는 `'counting_up`이라는 라벨이 붙어있고, 0에서 2까지 카운트한다. 라벨이 없는 안쪽 루프는 10에서 9까지 반대로 카운트한다.

라벨이 명시되지 않은 첫 번째 `break`는 안쪽 루프를 벗어나게 하며, 두 번째 `break`는 `break 'counting_up`을 사용하여 바깥쪽 루프를 벗어난다.

### `while` 을 이용한 조건 반복문

반복문을 실행할 때 조건식을 검사하여 조건이 true이면 계속 반복하고, 조건이 false이면 반복문을 종료하는 작업은 `loop`반복문과 `if`조건문, `break`키워드를 사용하여 구현할 수 있다.

그러나 이런 패턴은 매우 번거롭기 때문에 Rust에서는 `while`반복문이라 하는 구조를 제공하여 이 작업을 실행할 수 있다.

```rust
fn main() {
	let mut number = 2;
	
	while number != 0 {
		println!("{number}!");
		number -= 1;
	}
	println!("LIFTOFF!!");
}

// Output
//   Compiling test_rust v0.1.0 (C:\\Users\\rkdwl\\test_rust)
//    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.50s
//     Running `target\\debug\\test_rust.exe`
// 2!
// 1!
// LIFTOFF!!
```

이 구조는 `loop`, `if`, `else`, `break`를 사용할 때 필요하게 될 많은 복잡한 구조를 제거하고 코드를 더 깔끔하게 할 수 있도록 한다. `while`뒤의 조건식이 true로 판단되는 동안 코드가 실행되고, 그렇지 않으면 반복문을 벗어난다.

### `for` 를 이용한 컬렉션에 대한 반복문

배열의 각각의 요소들에 진입할 때 `while`반복문과 인덱스 변수를 따로 선언하여 사용할 수도 있지만, 조금 더 간편하게 `for`반복문을 사용하여 컬렉션의 각 아이템에 대해 코드를 수행할 수 있다.

```rust
fn main() {
	let a = [10, 20, 30, 40, 50];
	let mut index = 0;
	
	while index < 5 {
		println!("the value is: {}", a[index]);
		index += 1;
	}
}

fn main() {
	let a = [10, 20, 30, 40, 50];
	
	for element in a {
		println!("the value is: {element}");
	}
}

// Same Output
//    Compiling test_rust v0.1.0 (C:\\Users\\rkdwl\\test_rust)
//    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.34s
//     Running `target\\debug\\test_rust.exe`
// the value is: 10
// the value is: 20
// the value is: 30
// the value is: 40
// the value is: 50
```

이 경우, `while`반복문과 달리 우리가 배열의 요소를 변경하더라도 수정해야 할 다른 코드의 수가 적어지며, 배열의 끝을 넘어서거나 끝까지 가지 못해 몇 개의 아이템을 놓쳐서 발생할 수 있는 버그의 가능성을 원천적으로 제거할 수 있는 장점이 생긴다.

이러한 안전성과 간편성 덕분에 `for`반복문은 Rust에서 가장 흔하게 볼 수 있으며, 심지어 `while`반복문을 방금 전의 카운트다운 예제처럼 사용한 경우라도 대부분의 러스타시안들은 `for`반복문을 이용할 것이다.

표준 라이브러리가 제공하는 `Range`타입을 이용하면 특정 횟수만큼의 반복문을 구현할 수 있는데, `Range`는 어떤 숫자에서 시작하여 다른 숫자 종료 전까지의 모든 숫자를 차례대로 생성한다.

```rust
fn main() {
	for number in (1..4).rev() {
		println!("{number}!");
	}
	println!("LIFTOFF!!");
}

// Output
//    Compiling test_rust v0.1.0 (C:\\Users\\rkdwl\\test_rust)
//    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.54s
//     Running `target\\debug\\test_rust.exe`
// 3!
// 2!
// 1!
// LIFTOFF!!
```

## 요약

이제까지 변수, 데이터 타입, 함수, 구조체, 열거형, `if`표현식, 그리고 반복문에 대해 학습하였다.

학습한 개념들을 연습하고 싶다면 다음 프로그램 만들기에 도전해보자.

- 화씨 온도와 섭씨 온도 간 변환하기
- n번째 피보나치 수 생성하기
- 크리스마스 캐롤 ‘The Twelve Days of Christmas’ 노래의 반복성을 활용하여 가사 출력해보기

다음에는 다른 프로그래밍 언어에 흔치 않은 Rust의 개념인 소유권(ownership)에 대해 알아볼 것이다.