[[PNG의 구조|PNG 파일 포맷]]처럼, JPG 파일에도 파일 내의 빈 공간에 메시지나 파일을 숨길 수 있을 것이다. 

먼저 JPG 파일 샘플이 필요한데, 여기[^1]에서 다운받을 수 있으며, 750KB 이미지로 다운받았다. 
![[Pasted image 20250905074320.png|440x280]]
해당 이미지를 HxD로 열어 보면 
![[Pasted image 20250905074246.png]]
다음과 같은 사진이 나오며, [[메시지 은닉 알고리즘 파악]]에서 분석한 것처럼 파일의 시그니처가 `FF D8 FF E0`로 시작하는 것으로 보아 정상적인 JPG 파일인 것을 확인할 수 있다. 

### 전체적인 개요
- JPG 파일: **마커(Marker)** 들로 구성된 바이트 스트림으로, 각 마커는 `0xFF`로 시작하며 그 다음 바이트가 해당 마커의 종류를 나타냄
- 전형적인 흐름 (16진수 값: 해당 마커의 종류)
	1. SOI: Start Of Image
		- `0xD8`
		- 이미지의 시작을 나타냄
	2. (선택) APP0/APP1 등 메타데이터
		- JFIF의 경우, `0xE0`
		- EXIF의 경우, `0xE1`
		- 밑에서 더 자세하게 알아볼 것이다. 
	3. DQT: Define Quantization Table
		- `0xDB`
		- 이미지 압축에 사용되는 양자화 상수 테이블을 정의 
	4. SOF0: Start Of Frame
		- `0xC0`, (SOF2, 즉 Progressive DCT의 경우 `0xC2`)
		- 너비, 높이, 컴포넌트[^2] 등을 지정
	5. DHT: Define Huffman Table
		- `0xC4`
		- JPEG 이미지 압축 시 사용되는 [[DEFLATE 압축 알고리즘#2. Huffman Coding (통계적 부호화)|Huffman Coding]]을 위한 엔트로피 변환 테이블 정보
	6. SOS: Start Of Scan
		- `0xDA`
		- 스캔 시작을 의미하며, 실제 이미지의 스캔(압축) 데이터가 시작되는 지점
		- `0xDA`바로 뒤부터 실제 [[MCU]] 단위로 압축된 비트 스트림이 이어짐
	7. 엔트로피-부호화된 이미지 데이터
	8. EOI: End Of Image
		- `0xD9`
		- 이미지의 끝을 나타냄

이 때 DQT, SOF0, DHT 마커는 일종의 메타데이터를 정의하기 때문에 SOS 마커 앞에 나와야 하며, SOI, SOS, SOS, EOI 마커는 순서가 고정된다. 

### APP0 마커 구조
SOI와 그 뒤에서 나오는 다른 마커까지의 바이트 배열의 구조로, 다음 사진에서 파일 시그니처인 `FF D8 FF E0`다음으로 0x10 bytes를 차지하고 있다.
![[Pasted image 20250905023609.png]]

보통 JPG 파일을 HxD로 열어보면 해당 구조와 같은 JFIF (JPEG File Interchange Format) 형식을 따른다. 마커의 구조는 다음과 같다. 
- length (2 bytes): APP0 마커의 총 길이로, length 부분을 포함하며 APP0 마커 시그니처인 `0xE0`는 마커 길이 계산에 포함하지 않는다.
- identifier (5 bytes): `0x4A 0x46 0x49 0x46 0x00`으로 "JFIF" 문자열을 표현한다. 
- version (2 bytes): 상위 바이트는 주 버전을 나타내며, 하위 바이트는 부 버전을 나타낸다. 예를 들어, 버전이 1.02라면 `0x01 0x02`로 나타난다.
- units (1 bytes): 단위 당 몇 개의 점을 표현할지 = 해상도를 나타낸다. 
	- `0x00`: 단위 없음, 이 경우, 단순히 비율을 의미해서 Xdensity:Ydensity가 가로 세로 방향 픽셀 비율로 정의함
	- `0x01`: 단위: inch
	- `0x02`: 단위: cm
- Xdensity (2 bytes): 가로 방향의 해상도를 나타낸다.
- Ydensity (2 bytes): 세로 방향의 해상도를 나타낸다.
	- 예시: unit, Xdensity, Ydensity 순으로 `0x01 0x00 0x20 0x00 0x20`이라면 이미지를 인쇄할 때 1인치당 32개의 점(픽셀)을 사용
	- 이미지의 **물리적 크기를 결정** (이미지의 실제 픽셀 크기: SOF0에서 정의)
	- 화면에 표시될 때는 무시
- Xthumbnail (1 byte): Thumbnail(미리보기) 이미지의 가로 방향 픽셀 수
- Ythumbbail (1 byte): Thumbnail 이미지의 세로 방향 픽셀 수

### SOF 마커 구조
그 다음 나타나는 것은 SOFn 마커로, 일반적인 이미지에서 사용하는 SOF0 마커 외에도 Progressive JPEG에서 사용하는 SOF2와 Extended Sequential에서 사용하는 SOF1 마커 등이 존재하며 보통 SOF0와 SOF2가 가장 흔하게 사용된다.
![[Pasted image 20250906232430.png]]
- **SOF0 (`0xFF C0`)**: Baseline DCT (가장 널리 사용). (허프만 기반, 비예측)
- **SOF1 (`0xFF C1`)**: Extended sequential DCT (확장형 순차)
- **SOF2 (`0xFF C2`)**: Progressive DCT (프로그레시브 JPEG) — 여러 스캔으로 이미지 점진 전송
- **SOF3 (`0xFF C3`)**: Lossless (허프만 방식)

SOF는 JPEG 이미지의 가로, 세로 픽셀 크기와 색상 구성, 각 컴포넌트의 샘플링 정보, 이미지의 비트 길이 등 JPEG의 프레임 구조와 디코딩 방식을 정의한다. 

구조는 다음과 같다. 
- length (2 bytes): SOF 마커의 총 길이로, length 부분을 포함하며 SOF 마커 시그니처인 `0xC0`는 마커 길이 계산에 포함하지 않는다. 
- sample precision (1 byte): 보통 `0x08`
- image height (2 bytes)
- image width (2 bytes)
	- image height, width: Big-Endian으로 표시되며, 예시 이미지의 경우, 각각 `0x0500, 0x0780` = 1280, 1920으로 이미지의 규격과 동일한 것을 확인 가능하다. 
	![[Pasted image 20250906233755.png|450 x 445]]
- number of components (1 byte): `0x01` 또는 `0x03`
	- `0x01`: 그레이스케일 = 회색조 = 무채색
	- `0x03`: YCbCr 또는 RGB 색상
- component specification (각 component당 3 bytes)
	- component id (1 byte): 보통 Y = `0x01`, Cb = `0x02`, Cr = `0x03`
	- sampling factor (1 byte): 상위 4 bits = 수평 (Horizontal) 샘플링 팩터, 하위 4 bits = 수직(Vertical) 샘플링 팩터
		- 해당 컴포넌트가 [[MCU]]에서 얼마나 많은 8 x 8 블록을 차지할지 결정
		- 예시
			1. 먼저 Hmax, Vmax (프레임 내 모든 컴포넌트 중 가장 큰 H, V 값)를 구한다. 
			2. 그 컴포넌트의 실제 해상도는 다음처럼 줄어든다. 
				- 유효 해상도 (가로): `image_width * (Hc / Hmax)`
				- 유효 해상도 (세로): `image_height * (Vc / Vmax)`
			3. 즉, 샘플링 비율 = (Hc : Vc) / (Hmax : Vmax)
		- 적용 예시
			- **4:4:4 (무손실 다운샘플링 없음)**
			    - Y(H=1,V=1), Cb(H=1,V=1), Cr(H=1,V=1)
			    - 모든 컴포넌트가 같은 해상도.
	        - **4:2:2**
			    - Y(H=2,V=1), Cb(H=1,V=1), Cr(H=1,V=1)
			    - Y는 가로 해상도 2배, 색차는 절반.
			- **4:2:0 (가장 흔한 JPEG 샘플링)**
			    - Y(H=2,V=2), Cb(H=1,V=1), Cr(H=1,V=1)
			    - Y는 가로/세로 모두 2배, 색차는 1/4 해상도.
			    - MCU당 블록 배치: Y=4블록, Cb=1, Cr=1 → 총 6블록.
- Quantization table selector (1 byte): 해당 컴포넌트가 사용할 DQT 테이블 번호
	- 값의 범위는 `0x00 ~ 0x03`

### DQT 마커 구조
DQT 마커에서는 이미지 압축에 사용되는 양자화 테이블을 저장한다.
![[Pasted image 20250906232505.png]]
구조는 다음과 같다. 
- length (2 bytes): 마커의 총 길이로, length 부분을 포함하며 DQT 마커 시그니처인 `0xDB`는 마커 길이 계산에 포함하지 않는다. 
- Quantization Table Info (1 byte)
	- 상위 4 bits: `0x00` 또는 `0x01`의 값으로, 양자화 테이블의 값이 몇 비트로 저장될지 결정
		- `0x00`: 8 bit 양자화 값 > 테이블 안의 값이 0 ~ 255로, 가장 일반적인 값
		- `0x01`: 16 bit 양자화 값 > 테이블 안의 값이 0 ~ 65535로, 훨씬 정밀하게 양자화가 가능하지만 파일의 크기가 커져 잘 쓰이지 않는다. 
	- 하위 4 bits: `0x00 ~ 0x03`의 값으로, 어떤 양자화 테이블인지 구분하는 번호
		- `0x00`: Y (밝기 성분)
		- `0x01`: Cb, Cr (색차 성분)
			- 해당 이미지에서는 `0x00`과 `0x01`이 순서대로 사용되어 Y 채널과 Cb/Cr 채널에 대해 사용된 것을 알 수 있다. 
		- `0x02` 또는 `0x03`: 필요할 경우 추가로 사용
- Quantization Table Data (64 bytes): [[양자화 (Quantization)]]에 필요한 테이블이 저장되어 있다. 
-  양자화 테이블은 하나 이상 정의할 수 있다. (예시 이미지에서는 2개 정의)

### DHT 마커 구조
DHT 마커에서는 JPEG 이미지 압축 시 사용되는 [[DEFLATE 압축 알고리즘#2. Huffman Coding (통계적 부호화)|Huffman Coding]]을 위한 엔트로피 변환 테이블 정보를 저장한다. 스캔 시작을 의미하는 SOS 부분에서 각 컴포넌트가 어떤 DC/AC Huffman Table을 쓸지 참조하므로 **압축된 비트스트림을 해석하기 위해 이 테이블이 필요하다.**
![[Pasted image 20250906232542.png]]
구조는 다음과 같다. 
- length (2 bytes): 마커의 총 길이로, length 부분을 포함하며 DHT 마커 시그니처인 `0xC4`는 마커 길이 계산에 포함하지 않는다. 
- Huffman Table
	- Header (1 byte)
		- 상위 4 bits: 이 테이블이 DC(Direct Current)용인지, AC(Alternating Current)용인지
			- DC: 8 x 8 배열에서 첫 번째 값, 해당 블록이 대체로 밝은지 어두운지 결정하며, 값은 `0x00`
			- AC: DC를 제외한 나머지 63개의 값, 블록 안에서의 세부적인 패턴, 질감, 경계, 색의 변화 등을 표현하며, 값은 `0x01`
		- 하위 4 bits: 테이블 번호 (`0x00 ~ 0x03`)
	- 바이트 배열 (16 bytes):길이가 1 bit인 코드가 몇 개, 2 bits인 코드가 몇개, ..., 16 bits까지 코드가 몇 개인지 기록
	- Symbol List
		- 비트열인 코드에 대응되는 실제 값인 Symbol을 저장

[^1]: https://filesamples.com/formats/jpeg
[^2]: 컴포넌트는 색 공간에서의 색상 성분(RGB에서의 R, G, B, YCbCr에서의 Y, Cb, Cr)으로 이해해도 된다. 