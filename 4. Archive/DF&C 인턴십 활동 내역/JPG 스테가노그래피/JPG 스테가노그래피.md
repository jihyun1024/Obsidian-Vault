---
tags:
created: 2025-09-01
---
이번 2025년 2학기에는 기존의 PNG가 아닌 JPG 구조에 대해 수행되는 JPG 스테가노그래피에 대해 분석할 것이다. 

---
### JPG 구조 공부
JPG는 [[PNG의 구조|PNG]]와 파일의 구조가 다르기 때문에, 새로 JPG의 구조를 배워야 한다. 
JPG 파일의 구조를 배울 수 있는 사이트는 다음과 같다. 

- [ISO/IEC JPEG 표준 및 JFIF 규격](https://jpeg.org/jpeg/?utm_source=chatgpt.com)
- [JPEG FIF](https://www.w3.org/Graphics/JPEG/jfif3.pdf?utm_source=chatgpt.com)
- [The Metadata in JPEG files](https://dev.exiv2.org/projects/exiv2/wiki/The_Metadata_in_JPEG_files?utm_source=chatgpt.com)
- [JPG Signature Format: Documentation & Recovery Example](https://www.file-recovery.com/jpg-signature-format.htm?utm_source=chatgpt.com)

위의 사이트에서 나온 내용들을 바탕으로 [[JPG의 구조|JPG 파일의 구조]]에 대해 정리해 보았고, 
[[F5 Algorithm]]도 추가로 정리해 보았다. 

### 다양한 Steganography Algorithms
저번 세미나에서는 JPG의 자세한 구조와 F5 알고리즘의 대략적인 동작 방법에 대해 정리해 보았다. 그러나, 너무 피곤해서 그런가 F5 알고리즘에서 정작 중요한 메시지는 어떻게 은닉되는지, 그리고 어떻게 추출되는지에 대한 방법은 자세히 알아보지 못했던 것 같다. 

따라서 이번 세미나에서는 [[F5 Algorithm]]의 자세한 동작과 [[UERD Algorithm]]에 대해 알아보았다. 

### Feedback
[1차 피드백]
1. JPG 구조와 그에 관련된 개념에 대한 정리는 상당히 잘 해왔다.
2. F5 알고리즘에서 메시지를 어떻게 은닉하는지 추가로 더 알아와야 할 것 같다. 
3. 다음 세미나(2주 뒤)에서는 F5 알고리즘에서 어떻게 메시지를 은닉하는지, 그리고 UERD 알고리즘이 뭐고, 어떻게 동작하는지 알아오는 것이 과제

[2차 피드백]
1. 이런 종류의 논문 세미나를 진행할 때는 이 알고리즘이나 연구 방법들이 왜 나오게 되었는지, 어떻게 동작하는지, 그리고 처음 나오는 용어가 있다면 그게 어떤 것을 뜻하는지 이해하는 게 가장 중요하다. 
2. 스테가노그래피 주제 자체가 학기 중에 할 만하지는 않고, 다른 주제로 넘어가자. 