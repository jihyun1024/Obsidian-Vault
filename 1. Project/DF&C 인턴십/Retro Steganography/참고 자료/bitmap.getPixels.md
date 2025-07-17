https://developer.android.com/reference/android/graphics/Bitmap

Android의 [[Bitmap]] 클래스에서 제공하는 메서드로, 비트맵의 여러 픽셀을 한 번에 배열로 가져올 수 있는 기능을 제공한다. 
이는 `getPixel()`보다 훨씬 효율적이며, 이미지 처리나 분석 작업에서 자주 사용한다. 

### 주요 기능
* 다수의 픽셀을 한 번에 추출: 지정한 영역의 픽셀 데이터를 `int[]`배열에 저장한다. 
* ARGB 포맷: 각 픽셀은 `int`값으로 저장되며, `Color.red(), Color.green(), Color.blue()` 등을 통해 색상 성분을 분리할 수 있다. 

### 사용 예시
```java
int[] pixels = new int[width * height];
bitmap.getPixels(pixels, 0, width, x, y, width, height);
```
* `pixels`: 픽셀 데이터를 저장할 배열
* `offset`: 배열의 시작 위치
* `stride`: 가로줄 한 줄당 픽셀 수
* `x, y`: 시작 좌표
* `width, height`: 추출할 영역의 크기

### 주의 사항
* 배열의 크기가 충분하지 않으면 `ArrayIndexOutOfBoundsException`이 발생할 수 있다. 