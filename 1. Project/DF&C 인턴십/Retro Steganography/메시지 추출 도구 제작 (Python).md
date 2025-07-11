```# Jadx의 Java 코드와 최대한 비슷한 로직으로 구현

import os
import io

# CRC 검증 및 데이터 압축에 사용하는 모듈
import zlib
import gzip

# 이미지 처리에 사용하는 모듈
from PIL import Image

f6245a = b'\x0a\x47\x70\x4b\x08\x10\x10\x0e\xe2\x93\xff\x08\xe6\x3e\xcf\xd8\x1b\x67\x3f\xab\xe0\x7c\x2d\x12\x11\x93\x39\xf8\xc6\xa7\xff\x28'

# 메시지 추출 로직: LSB + XOR + GZIP
def message_decode(image):
    # PIL 모듈에서는 RGBA의 순서로 픽셀의 색상을 다룸
    pixels = list(image.convert('RGBA').getdata())
    width, height = image.size
    total_pixels = width * height

    if total_pixels < 8:
        return None
    
    # Android는 Java 기반이기 때문에 ARGB 배열로 변환하여 픽셀 리스트를 재구성
    # Java에서는 픽셀을 32bit 정수 하나로 표현하기 때문에 각 색상 값을 ARGB에 맞게 비트 시프트하여 표현
    pixel_data = []
    for r, g, b, a in pixels:
        argb = (a << 24) | (r << 16) | (g << 8) | b
        pixel_data.append(argb)

    # 원소 6개의 바이트 배열을 구성
    bArr2 = bytearray(6)
    i6 = -1
    i7 = 0
    i8 = 0
    i9 = 0
    i10 = 0
    i11 = 0

    # 반복문을 돌리며 은닉된 메시지 추출
    while True:
        if i6 == -1 or i7 < i6 + 6 + 2:
            i13 = 0

            # 1byte = 8bit이므로 LSB 방식으로 메시지를 추출
            for i in range(8):
                # i % 3 == 0인 경우, Blue 채널
                # i % 3 == 1인 경우, Green 채널
                # i % 3 == 2인 경우, Red 채널에서 메시지 추출 -> RGB를 반복적으로 돌면서 추출
                i15 = 1 << (16 - ((i % 3) * 8)) 
                i16 = 1 if i in (2, 5, 7) else 0
                i17 = pixel_data[i11]

                # 위에서 결정된 채널의 LSB 추출, 이를 8번 반복하면 i13에는 1바이트의 정보가 완성됨
                z = (i17 & i15) != 0
                i13 >>= 1

                if z:
                    i13 |= 0x80
                i9 += i16
                i11 += i16
                
                if i9 >= width:
                    i10 += 1
                    i9 = 0
                    if i10 >= height:
                        return None
                i18 = i17
            bArr2[i7] = i13

            if (i18 & 1) != ((bArr2[i7] ^ i7) & 1):
                break
            if i7 == 5:
                # bArr2의 첫 번째와 두 번째가 0xF6, 0xD1이 아닌 경우, break
                # 굳이 0xF6, 0xD1인 이유: 앱에서 그렇게 정했으니까?
                if bArr2[0] != 246 or bArr2[1] != 209:
                    break
                i8 = bArr2[2]
                i6 = (bArr2[3] << 16) | (bArr2[4] << 8) | bArr2[5]
                bArr2 += bytearray(i6 + 2)
            
            i7 += 1
        else:
            # CRC 값 검증
            CRC_in_data = (bArr2[-2] << 8) | bArr2[-1]
            CRC_calculate = zlib.crc32(bArr2[:-2]) & 0xFFFFFFFF
            CRC_check = (CRC_calculate ^ (CRC_calculate >> 16)) & 0xFFFF

            if CRC_in_data == CRC_check:
                # XOR 기반으로 키처럼 복호화 비슷하게 수행
                if i8 & 0x10:
                    XOR_decrypt(bArr2, 6, i6, f6245a)

                message_in_image = bArr2[6:6+i6]

                # GZIP 압축 여부 확인, 압축된 경우 압축 해제 수행
                if i8 & 0x40:
                    try:
                        with gzip.GzipFile(fileobj=io.BytesIO(message_in_image)) as f:
                            message = f.read()
                        # 문자 한 두 문자정도 오류났을 때에는 다른 문자로 대체
                        return message.decode('utf-8', errors='replace')
                    except:
                        return 'Error: Failed to decompressing!'
                else:
                    return message_in_image.decode('utf-8', errors='replace')
            break
    return None


# XOR 연산을 기반으로 키처럼 복호화 비슷한 것을 수행 (편의상 타입 힌트 사용)
def XOR_decrypt(data: bytearray, i1: int, i2: int, key: bytes):
    # 키가 존재하지 않을 경우: return
    if not key:
        return
    else:
        key_length = len(key)
        i3 = 0
        for i4 in range(i2):
            # 데이터와 키를 XOR 연산
            data[i1 + i4] = (key[i3] ^ i4) ^ data[i1 + i4]
            # Jadx 코드에서의 삼항 연산자는 Modulo 연산과 동일
            i3 = (i3 + 1) % key_length


# 메인 함수 (폴더 경로 입력 후 .jpg와 함께 파일 이름 입력)
if __name__ == '__main__':
    print('Retro Steganography Decoding...')
    filepath = input("Enter file's folder path: ")
    if not os.path.exists(filepath):
        print('Error: Folder does not exists!')
        exit()
    
    os.chdir(filepath)
    filename = input('Enter image file name with file with .JPG or .JPEG: ')
    try:
        if not filename.lower().endswith(('.jpg', 'jpeg')):
            print('Error: Only JPG or JPEG supported!')
            exit()
        if not os.path.exists(filename):
            raise FileNotFoundError
        
        filename = os.path.join(filepath, filename)

        
        image = Image.open(filename)
        decoded_message = message_decode(image)
        if decoded_message:
            print('Decoded Message: ', decoded_message)
    except FileNotFoundError:
        print('Error: File does not found!')
```
