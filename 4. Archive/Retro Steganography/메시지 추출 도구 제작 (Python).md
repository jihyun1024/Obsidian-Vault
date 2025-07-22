```python fold-Retro_Steganography_Decoder
import os
import io
import zlib
import gzip
from PIL import Image


# XOR 복호화용 키
# Java의 byte는 signed(부호 있는 형태)이지만, Python은 unsigned이므로
# -1 같은 음수 값을 변환해야 함. (변환 공식: 256 + (Java에서의 음수값) = Python에서의 unsigned값)
XOR_KEY = bytes([
    10, 71, 112, 75, 8, 16, 16, 14,
    226, 147, 127, 8, 230, 62, 207, 216,
    27, 103, 63, 171, 224, 124, 45, 18,
    17, 147, 57, 248, 198, 167, 255, 40
])
  

# LSB 추출, CRC 계산 이후 추출된 바이트에 XOR을 수행하여
# 복호화 비슷한 것을 수행하는 함수 (그 이후, GZIP 압축 해제를 실행함)
def xor_decrypt(data: bytearray, offset: int, length: int, key: bytes):
    if not key:
        return
    else:
        key_len = len(key)
        for i in range(length):
            data[offset + i] ^= (key[i % key_len] ^ i)
  

# 이미지에서 숨겨진 데이터를 추출하는 함수 (핵심 기능을 수행하는 함수)
def extract_message(image: Image):
    # 처음에는 RGBA로 이미지에서 픽셀 데이터를 받아 옴.
    pixels = list(image.convert('RGBA').getdata())
    width, height = image.size
    total_pixels = width * height
  
    # 이미지에 숨겨진 데이터가 8바이트도 되지 않을 경우 None을 리턴
    if total_pixels * 3 // 8 < 8:
        return None
  
    # Java는 RGBA를 쓰지만, Python은 ARGB를 쓰기 때문에 RGBA를 ARGB 값으로 변환
    pixel_data = [(a << 24) | (r << 16) | (g << 8) | b for r, g, b, a in pixels]

  
    # buffer: 여러 데이터(메시지 압축 여부, 은닉된 메시지의 길이, 은닉된 원본 데이터 등등)가 들어있는 바이트 배열
    buffer = bytearray(6)
    found_header = False    # 헤더를 읽었는지 여부
    message_length = 0      # 메시지의 길이
    flags = 0               # 압축이나 XOR 암호화가 되었는지에 대한 플래그
    index_in_buffer = 0     # 현재 버퍼에 채운 바이트 수
    x = y = pixel_index = 0 # 이미지 탐색을 수행하면서 사용하는 인덱스
  
    while True:
        if not found_header or index_in_buffer < message_length + 8:
            byte = 0
            for bit_index in range(8):
                # R, G, B 순서로 LSB 추출
                color_shift = 16 - ((bit_index % 3) * 8)
                pixel = pixel_data[pixel_index]
                bit = (pixel >> color_shift) & 1
  
                # 비트를 오른쪽부터 채워서, 즉 처음에는 MSB 위치에 있었다가 점점 LSB로 내려가는 식으로 1바이트 구성
                byte >>= 1
                if bit:
                    byte |= 0x80
  
                # 2,5,7번째 비트에서는 다음 픽셀로 이동, 이미지 범위 초과 시 None을 리턴
                if bit_index in (2, 5, 7):
                    x += 1
                    pixel_index += 1
                    if x >= width:
                        y += 1
                        x = 0
                        if y >= height:
                            return None
  
            # 만약 현재 버퍼에 채운 바이트 수가 현재 버퍼의 길이보다 크다면 늘려줌
            if index_in_buffer >= len(buffer):
                buffer.extend([0] * (message_length + 2))
  
            buffer[index_in_buffer] = byte
  
            # 패리티 확인(오류 방지를 위한 검증)
            # 마지막에 읽은 픽셀의 LSB와 (byte ^ index) & 1을 비교해서 일치하지 않으면 break
            if (pixel & 1) != ((byte ^ index_in_buffer) & 1):
                break
                
            # 6번째 바이트까지 읽은 경우
            if index_in_buffer == 5:
                if buffer[0] != 0xF6 or buffer[1] != 0xD1: # 일종의 시그니처
                    break

                flags = buffer[2] # 메시지가 압축 또는 XOR 암호화 되었는지에 대한 플래그
                message_length = (buffer[3] << 16) | (buffer[4] << 8) | buffer[5]
                buffer.extend([0] * (message_length + 2))
                found_header = True # 헤더를 모두 설정했다고 치고, 헤더 뒤부터는 본문 데이터가 나옴.
  
            index_in_buffer += 1
        else:
            # CRC32 검증 시작
            # crc_in_data: 버퍼, 즉 입력받은 이미지에 있던 CRC 체크섬
            # crc_calculated: CRC32 알고리즘으로 계산한 CRC 체크섬
            crc_in_data = (buffer[-2] << 8) | buffer[-1]
            crc_calculated = zlib.crc32(buffer[:-2]) & 0xFFFFFFFF
            crc_short = (crc_calculated ^ (crc_calculated >> 16)) & 0xFFFF
  
            # CRC가 검증된 경우에만 XOR 복호화 또는 GZIP 압축 해제 실행
            if crc_in_data == crc_short:
                # 만약 플래그 & 0x10 = true인 경우, 메시지는 XOR로 암호화 되었다고 가정하고 복호화
                if flags & 0x10:
                    xor_decrypt(buffer, 6, message_length, XOR_KEY)

                # 메시지 은닉에서도 알 수 있듯이, 숨겨진 데이터는 buffer의 6번째 인덱스에 저장
                hidden_data = buffer[6:6 + message_length]
  
                # 플래그 & 0x40 = true인 경우, 메시지는 GZIP으로 압축되었다고 가정하고 압축 해제 실행
                if flags & 0x40:
                    try:
                        with gzip.GzipFile(fileobj=io.BytesIO(hidden_data)) as gz:
                            return gz.read().decode('utf-8', errors='replace')
                    except Exception:
                        return 'Error: Failed to decompress message!'
                else:
                    return hidden_data.decode('utf-8', errors='replace'), message_length
            break
    return None
  

# 파일 경로와 파일 이름을 입력받아 메시지 추출 함수를 호출
def main():
    print('Retro Steganography Decoding...')
    folder_path = input("Enter folder path: ").strip()
    if not os.path.isdir(folder_path):
        print('Error: Folder does not exist!')
        return
  
    os.chdir(folder_path)
    file_name = input("Enter image file name (with .JPG or .JPEG): ").strip()
    if not file_name.lower().endswith(('.jpg', '.jpeg')):
        print('Error: Only JPG or JPEG files are supported!')
        return
    if not os.path.isfile(file_name):
        print('Error: File not found!')
        return

    try:
        with Image.open(file_name) as image:
            message, length = extract_message(image)
            if message:
                print('Decoded Message: ', message)
                print('Message Length (byte): ', length)
            else:
                print('Error: No valid hidden message found.')
    except Exception as e:
        # 오류가 발생했을 경우를 대비한 오류 메시지
        print(f'Error: Failed to process image ({e})')
  

if __name__ == '__main__':
    main()
```
---
### 만들면서 헷갈렸던/어려웠던 것
1. XOR에 사용하는 키를 처음에는 16진수 음수표현으로 해야 하나 생각했는데, [[Python 메인 문서|Python]]에는 16진수 음수표현 대신 `bytes([10진수 숫자])` 이렇게 표현하고 음수는 `256 - (Java에서의 음수값)`으로 10진수로 나타내면 된다는 것을 이번 프로젝트를 통해 알 수 있었다.
2. 변수명을 설정하기 너무 어려워서 AI의 도움을 많이 받을 수 밖에 없었다.
3. Java와 Python의 PIL에서 RGB를 표현하는 방식이 ARGB와 RGBA로 각각 다른 것 때문에 변환하는 코드가 있어야 한다는 것을 알았다. 