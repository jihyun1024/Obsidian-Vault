[BufferdInputStream (Java Platform SE 8)](https://docs.oracle.com/javase/8/docs/api/java/io/BufferedInputStream.html)

입력 스트림에서 데이터를 더 효율적으로 읽기 위해 버퍼를 사용하는 스트림이다. 
즉, 매번 한 바이트씩 읽는 대신, 한 번에 여러 바이트를 메모리 버퍼에 저장해 놓고 필요할 때마다 그 버퍼에서 데이터를 꺼내 사용하는 방식으로 동작한다. 
이 방식을 사용하여 입출력 성능을 크게 높일 수 있다. 
### 필드 요약
* `pretected byte[] (buf)`: 입력 스트림에서 읽은 데이터를 저장하는 버퍼
* `protected int (count)`:`buf` 배열에 실제로 저장된 데이터의 총량
* `protected int (marklimit)`: 마크 이후 읽을 수 있는 최대 바이트 수
* `protected int (markpos)`: mark 메서드로 설정한 마크의 위치로, reset 메서드를 호출하면 이 위치로 되돌아가며, 마크가 설정되지 않았을 경우는 -1
* `protected int (pos)`: 현재 읽고 있는 버퍼 내의 위치
### 생성자 요약
* `BufferedInputStream(Inputstream in)`: 원본 입력 스트림 `in`을 가지고 BufferedInputStream 객체를 생성
* `BufferedInputStream(Inputstream in, int size)`: 원본 입력 스트림 `in`을 가지고 PushbackInputStream 객체를 주어진 크기 `size`만큼으로 생성 (기본값: 8192 바이트)
### 주요 메서드
* `int read()`: 한 바이트를 읽음
* `int read(byte[] b, int off, int len)`: 바이트 배열에 데이터를 읽음
* `void close()`: 스트림을 닫고 버퍼를 해제함
* `int available()`: 읽을 수 있는 바이트의 수를 반환함
* `void mark(int readlimit)`: 현재 위치를 마크함
*  `void reset()`: 마크한 위치로 되돌림
* `boolean markSupported()`: 마크 기능을 지원하는지의 여부를 반환함 (항상 True)
### 사용
파일이나 네트워크에서 데이터를 읽을 때 성능 향상을 위해 사용하며, 그 외에도 많은 양의 데이터를 반복적으로 읽을 때, 특정 위치를 기억해 두고 다시 돌아가야 할 때 등의 상황에서 사용한다. 