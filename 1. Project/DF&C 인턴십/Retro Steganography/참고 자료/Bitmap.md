[Bitmap | API Reference](https://developer.android.com/reference/android/graphics/Bitmap)
[비트맵 - 위키백과, 우리 모두의 백과사전](https://ko.wikipedia.org/wiki/%EB%B9%84%ED%8A%B8%EB%A7%B5)

이미지를 처리할 때의 Bitmap은 간단히 말해 서로 다른 점(픽셀)들의 조합으로 그려지는 이미지 표현 방식이다. Bitmap 이미지는 정교하고 화려한 표현이 가능하며, 주로 사진 이미지에 사용된다. 

화면 상의 각 점들을 데카르트 좌표계를 통해 화소 단위로 나타내며, 그림을 확대하면 각 점들이 그대로 커져 경계 부분이 오돌토돌하게 보이는 계단 현상이 나타난다. 이를 조금 더 부드럽게 처리하기 위한 알고리즘들(쌍삼차 필터링, 이중선형 필터링)이 있다. 

가로 x 세로 만큼의 픽셀 데이터를 전부 저장해야 하기 때문에 벡터 방식(수학 방정식을 기반으로 점, 직선, 곡선, 다각형 등의 이미지를 사용하는 방식)의 이미지나 텍스트 자료에 비해 상대적으로 용량이 크고 처리 속도가 느리다. ([[메시지 추출 도구 제작 (Python)]]에서 시간이 오래 걸린 이유)
이를 개선하기 위해 JPEG, GIF, PNG 등의 다양한 파일 형식이 개발되었다. 

---
# 📸 `android.graphics.Bitmap` 클래스 정리

> Android에서 비트맵 이미지를 표현하고 조작하는 데 사용되는 클래스. `Parcelable`을 구현하며, 다양한 생성 및 변환 메서드를 제공.

---
## 📌 기본 정보

- **패키지**: `android.graphics`
- **상속**: `java.lang.Object` → `android.graphics.Bitmap`
- **구현 인터페이스**: `Parcelable`
- **추가된 API 레벨**: 1

---
## 🧩 주요 상수

|상수|설명|
|---|---|
|`DENSITY_NONE`|픽셀 밀도가 알려지지 않은 비트맵을 나타냄|

---
## 🧰 주요 메서드

### 📤 저장 및 복사

- `boolean compress(format, quality, stream)`
    - 비트맵을 지정된 포맷과 품질로 압축하여 출력 스트림에 저장
- `Bitmap copy(config, isMutable)`
    - 현재 비트맵을 복사하여 새 비트맵 생성

### 📥 버퍼와의 상호작용

- `void copyPixelsFromBuffer(Buffer src)`
- `void copyPixelsToBuffer(Buffer dst)`

### 🧱 비트맵 생성 (정적 메서드)

- `createBitmap(Bitmap source, int x, int y, int width, int height)`
- `createBitmap(int[] colors, int width, int height, Bitmap.Config config)`용
- `BitmapFactory`: 다양한 소스로부터 비트맵을 생성하는 유틸리티 클래스
- `Canvas`: 비트맵에 그리기 위한 클래스
- `Matrix`: 비트맵 변환(회전, 스케일 등)에 사용

---
## 🧠 사용 팁

- **메모리 관리 중요**: 큰 이미지를 다룰 때는 `BitmapFactory.Options`의 `inJustDecodeBounds`를 활용해 메모리 초과 방지
- **공유 메모리**: `asShared()`를 통해 프로세스 간 효율적인 비트맵 전달 가능