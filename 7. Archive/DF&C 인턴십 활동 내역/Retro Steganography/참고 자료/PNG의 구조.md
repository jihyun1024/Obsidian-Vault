Chunk: PNG 이미지 파일을 구성하는 기본 단위로, PNG에는 4 부분의 Chunk가 존재하며, 빨간색으로 표시된 Chunk들은 필수적으로 존재하는 Chunk들이다.

- 파일 시그니처: 89 50 4E 47 0D 0A 1A 0A
    
- **IHDR** : PNG 파일의 시작을 나타내는 Chunk
    
    - Length (4byte): IHDR Chunk에서 시그니처와 CRC를 제외한 데이터 영역의 길이
        
    - Width, Height (각각 4byte): PNG 파일의 가로/세로 길이를 나타낸다. (범위: 0 ~ $2^{31} - 1$)
        
    - Bit depth (1byte): 1, 2, 4, 8, 16 중 하나의 값으로, 각 픽셀이 표현할 수 있는 색상 정보 또는 픽셀당 비트의 수이며, Color type과 함께 작동한다.
        
    - Color type (1byte): 이미지의 색상 구성 방식과 알파(투명도) 채널 포함 여부에 대한 정보
        
|**Color Type**|**설명**|**구성**|**사용 가능한 Bit Depth**|
|----|----|----|----|
|0|Grayscale|회색조만 (흑백)|1, 2, 4, 8, 16|
|2|Truecolor|RGB (색상)|8, 16|
|3|Indexed-color|팔레트 기반 색상|1, 2, 4, 8|
|4|Grayscale + Alpha|회색조 + 투명도|8, 16|
|6|Truecolor + Alpha|RGB + 투명도|8, 16|
        
-  예: Color type = 6, Bit depth = 8 → R,G,B,A 4개의 채널당 각 8비트 ⇒ 픽셀당 32비트
    
- Compression method (1byte): 이미지 데이터를 압축하는 데 사용된 방식에 대한 정보로, 모든 PNG 파일은 0을 사용한다.
        
    - Filter method (1byte): 압축 전에 이미지 데이터를 처리하는 방식으로, 모든 PNG 파일은 0을 사용한다.
        
    - Interlace method(1byte): 데이터의 전송 순서를 나타내는 정보로, 0 또는 1을 사용한다.
        
        |**Interlace Method**|**이름**|**설명**|
        |---|---|---|
        |0|None|인터레이싱 없음 (정상적으로 한 줄씩)|
        |1|Adam7|Adam7 인터레이싱|
        
    - CRC (4byte): 데이터 오류 검증에 사용
        
- PLTE: IHDR의 Color type = 3(팔레트 기반)일 때만 존재
    
- sRGB: RGB 색 공간을 지정
    
    효과: 해당 Chunk가 존재하면 PNG 뷰어/디코더는 sRGB 색공간을 강제로 적용하며, gAMA와 cHRM Chunk가 존재해도 두 Chunk는 무시된다.
    
    - Length (4byte): sRGB Chunk에서 시그니처와 CRC를 제외한 데이터 영역의 길이
        
    - Rendering intent (1byte): 색상 렌더링 방식을 저장
        
| Intent value | 이름                   | 특징                                   | 용도            |
| ------------ | -------------------- | ------------------------------------ | ------------- |
| 0            | Perceptual           | 자연스러운 색 보정, 전체 이미지 조정                | 일반 사진         |
| 1            | Relative Colormetric | 재현 가능한 색은 정확하게, 나머지는 조정              | 로고, 인쇄        |
| 2            | Saturation           | 채도 보존 중심                             | 비즈니스 그래픽      |
| 3            | Absolute Colormetric | 색상의 절대 좌표 유지, 백색점(White point) 또한 유지 | 색상 교정, 인쇄 테스트 |
        
- CRC (4byte): 데이터 오류 검증에 사용
        
- gAMA: 감마(이미지의 밝기) 정보를 지정
    
    - Length (4byte): gAMA Chunk에서 시그니처와 CRC를 제외한 데이터 영역의 길이
        
    - Gamma (4byte): 부동소수점이 아닌 정수로 저장되며
        
        $gamma = {gAMA 청크 값} / 100000$ 으로 환산하여 계산
        
    - CRC (4byte): 데이터 오류 검증에 사용
        
- pHYs: 픽셀의 물리적인 크기 정보를 지정, Big-Endian으로 해상도를 저장
    
    - Length (4byte): pHYs Chunk에서 시그니처와 CRC를 제외한 데이터 영역의 길이
    - Pixels per unit, X-axis (4byte): X 방향의 픽셀 해상도
    - Pixels per unit, Y-axis (4byte): Y 방향의 픽셀 해상도
    - Unit specifier (1byte): 단위 지정
        - 00: 단위 없음
        - 01: 미터 단위
    - CRC (4byte): 데이터 오류 검증에 사용
- tEXt / zTXt / iTXt: 텍스트 메타데이터를 저장
    
| **키워드**         | **의미**            |
| --------------- | ----------------- |
| `Title`         | 이미지 제목            |
| `Author`        | 이미지 제작자           |
| `Description`   | 이미지 설명            |
| `Copyright`     | 저작권 정보            |
| `Creation Time` | 생성 시간             |
| `Software`      | 이미지 생성에 사용된 소프트웨어 |
| `Disclaimer`    | 면책 조항             |
| `Warning`       | 경고 메시지            |
| `Source`        | 이미지 출처            |
| `Comment`       | 일반적인 코멘트          |
    
    
- Keyword: 메타데이터의 종류나 목적을 설명하는 필드로, 1 ~ 79byte의 Latin-1 문자로 구성되며, 널 문자로 끝난다.
    
    - tEXt: 일반 텍스트 메타데이터
    - zTXt: 압축된 텍스트 메타데이터
    - iTXt: UTF-8을 지원하는 메타데이터
- tRNS: 투명도 정보를 저장하는 데 사용하는 보조 Chunk
    
    투명도 정보(알파 채널)를 지정하지 않는 Color type 0, 2, 3에서만 사용하는 보조 Chunk로, 각각의 Color type에 대해 다음과 같이 동작한다:
    
    1. Grayscale (Color type 0)
        - 형식: 투명하게 만들고 싶은 값 2바이트
        - 예시: 00 80 → 128값의 회색을 투명하게 설정
    2. Truecolor RGB (Color type 2)
        - 형식: 투명하게 만들고 싶은 RGB 색상 각 2바이트씩 총 6바이트
        - 예시: 00 FF 00 FF 00 FF → 순수 흰색(R = 65535, G = 65535, B = 65535)를 투명하게 설정
    3. Indexed-color (Color type 3)
        - 형식: 팔레트 인덱스별 투명도 값 (1바이트씩)
        - 예시: FF FF FF 00 → 팔레트 인덱스 3번은 투명, 나머지는 불투명하게 설정
- **IDAT**: 이미지의 실제 데이터를 포함하는 Chunk
    
    IDAT Chunk는 여러 개 존재할 수 있으며, 각 Chunk마다 Chunk length, IDAT identifier, 실제 이미지 데이터, CRC 값의 순서로 데이터가 존재한다.
    
    - IDAT의 이미지 데이터를 만드는 순서
        1. 스캔라인 (이미지의 가로 방향 한 줄의 픽셀 데이터) 단위로 이미지 데이터를 필터링
        2. 필터링한 결과를 IHDR에 명시된 압축 방식으로 압축 후 IDAT에 저장
    - IDAT의 이미지 데이터를 읽는 순서: 만드는 순서와 정반대로 실행
- **IEND**: PNG 데이터의 끝을 나타내는 Chunk로, 4바이트의 0, IEND 문자열과 CRC 값만 존재
    