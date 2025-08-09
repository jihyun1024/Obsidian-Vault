### PNG 파일 구조
[[PNG의 구조]] 문서에 잘 정리되어 있으니 참고하자. 

### LSB 코드 작성
```python
import os
import numpy as np
from PIL import Image


def stego_hide(filepath, msg):
    try:
        image = Image.open(filepath).convert('RGB')
        pixel = np.array(image)
        
        # 메시지 인코딩 후, 그 끝에 메시지의 끝을 알리는 0x00 추가
        hide = msg.encode('utf-8') + b'\x00'
        hide_bits = ''
        for byte in hide:
            hide_bits += format(byte, '08b')

        # LSB 기법 사용, 메시지 은닉
        what = 0
        for i in range(pixel.shape[0]):
            for j in range(pixel.shape[1]):
                for k in range(3): # 픽셀 하나의 길이 = 3
                    if what < len(hide_bits):
                        pixel[i][j][k] = int(format(pixel[i][j][k], '08b')[:-1] + hide_bits[what], 2)
                        what += 1
                    else:
                        break
  
        # 이미지는 filepath로 받은 경로에 저장됨
        newImage = Image.fromarray(pixel)
        newImage.save('FaS.png')
        print('Successfully hid the message!')

    except FileNotFoundError:
        print('File does not exist!')


def stego_seek(filepath):
    try:
        image = Image.open(filepath).convert('RGB')
        pixel = np.array(image)

        hide_bits = ''
        for i in range(pixel.shape[0]):
            for j in range(pixel.shape[1]):
                for k in range(3): # 픽셀 하나의 길이 = 3
                    hide_bits += format(pixel[i][j][k], '08b')[-1]

        # hide_bits를 8비트 단위로 나누기
        hide_bytes = []
        for i in range(0, len(hide_bits), 8):
            hide_bytes.append(hide_bits[i:i + 8])

        # 각 8비트 문자열을 바이트로 변환
        message = bytearray()
        for byte in hide_bytes:
            message.append(int(byte, 2))

        # bytes 객체로 변환 및 utf-8로 디코딩
        message = bytes(message)
        message = message.split(b'\x00')[0].decode('utf-8')
        print('찾은 메시지는 ' + message + ' 입니다')

    except FileNotFoundError:
        print('File does not exist!')


if __name__ == '__main__':
    select = int(input('Enter 0 to hide, enter 1 to seek: '))

    if select == 0:
        path = input('Enter directory of your image to hide message: ')
        os.chdir(path)
  
        filepath = os.getcwd() + '\Soonwook Binchanhap.png'
        message = input('Enter message to hide: ')
        stego_hide(filepath, message)
    elif select == 1:
        path = input('Enter directory of your image to seek message: ')
        os.chdir(path)

        filepath = os.getcwd() + '\FaS.png'
        stego_seek(filepath)
    else:
        print('Please enter 0 or 1')
```