[PushbackInputStream (Java Platform SE 8)](https://docs.oracle.com/javase/8/docs/api/java/io/PushbackInputStream.html)

데이터의 읽기복구(Unread) 기능을 제공하는 바이트 입력 필터로, 방금 읽은 한 바이트를 읽기 전의 바이트 입력 스트림으로 되돌려 놓는 것을 가능하게 한다. 
코드 조각이 특정 바이트 값으로 구분된 무한한 개수의 데이터 바이트를 읽어야 하는 상황에서 유용하다. 
종료 바이트를 읽은 후, 코드 조각은 해당 바이트를 "읽지 않음"으로 설정할 수 있으므로 입력 스트림에 대한 다음 읽기 작업에서 Pushback된 바이트를 다시 읽어온다.
예를 들어 식별자를 구성하는 문자를 나타내는 바이트는 연산자 문자를 나타내는 바이트로 끝날 수 있으며, 식별자만 읽어오는 것이 목적이라면 연산자를 볼 때까지 읽어온 후 연산자를 Pushback해서 다시 읽을 수 있다. 
### 필드 요약
* `Protected byte[] (buf)`: Pushback에 사용할 버퍼
* `Protected int (pos)`: Pushback 버퍼 내에서 다음 바이트가 읽혀질 위치
### 생성자 요약
* `PushbackInputStream(InputStream in)`: 원본 입력 스트림 `in`을 가지고 PushbackInputStream 객체를 생성
* `PushbackInputStream(InputStream in, int size)`: 원본 입력 스트림 `in`을 가지고 PushbackInputStream 객체를 주어진 크기 `size`만큼으로 생성
### 주요 메서드
* `int read()`: 한 바이트를 읽음
* `int read(byte[] b, int off, int len)`: 바이트 배열에 데이터를 읽음
* `void unread(int b)`: 한 바이트를 다시 스트림에 밀어 넣음
* `void unread(byte[] b)`: 바이트 배열 전체를 다시 밀어 넣음
* `void unread(byte[] b, int off, int len)`: 배열의 일부를 다시 밀어 넣음
* `int available()`: 읽을 수 있는 바이트 수 반환
* `void close()`: 스트림을 닫음
### 사용
특정 패턴을 읽다가 조건이 맞지 않으면 다시 되돌려야 할 때, 다음 문자를 미리 보고 판단한 후 다시 되돌려야 할 때, 또는 XML, JSON, CSV 등을 사용할 때 유용하게 사용할 수 있다. 