UERD 알고리즘은 JPEG 파일에 데이터를 은닉하는 스테가노그래피 기법으로, **Guo, L., Ni, J., Su, W., Tang, C., Shi, Y. Q.** 가 공동 저자로 참여한 논문에서 처음 제안되었고. 이 논문은 **2015년**에 **IEEE Transactions on Information Forensics & Security** 저널에 발표되었다. 
발표한 논문은 [이 사이트](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7225122)에서 PDF 파일로 확인할 수 있다. 
# UERD 알고리즘 동작 원리

**출처**: Guo et al., *Using Statistical Image Model for JPEG Steganography: Uniform Embedding Revisited*, IEEE TIFS 2015

---

## 1. 배경
- **UED (Uniform Embedding Distortion)**  
  - 모든 크기의 DCT 계수에 균등하게 변경을 퍼뜨려 1차/2차 통계 변화 최소화  
  - 하지만 실제 이미지 통계적 모델에서 "상대적 변화"를 고려하지 않아 최적은 아님  

- **UERD (Uniform Embedding Revisited Distortion)**  
  - **상대적 변화량**(Coefficient of Variation, CV)에 비례해 변경을 분산  
  - DC, Zero AC, Non-zero AC 계수를 모두 커버 요소로 사용  
  - 블록 복잡도와 모드별 특성을 함께 반영 (Block 기반 비용과 Mode 기반 비용 전부 계산)

---

## 2. 핵심 개념
### 2.1 변동계수(CV, Coefficient of Variation)
- JPG 이미지는 8 x 8 블록 [[DCT]]를 사용 → 각 블록에서 동일 위치($u,v$)의 계수들을 모으면 **mode**
	(예시: (1,2) 위치의 계수들을 모든 DCT 블록에서 모으면 (1,2) 모드 계수 집합)
- 모드마다 계수 값들이 분포(히스토그램)를 이룸 
	- 밑의 그림(왼쪽)처럼 0이 가장 많이 분포
	- **모드별로 따로 계수 히스토그램을 만듦 (총 8 x 8 = 64개)**
- 이 때 값들은 계수값들의 평균/표준편차가 아니라 **빈도(계수값이 몇 번이나 많이 나오는지)값의 평균/표준편차**

- 정의: 그림으로 추가로 이해 가능 **(DC 계수쪽일수록 메시지 은닉에 불리)**
  
  $$CV(x) = \frac{\sigma(x)}{\mu(x)}
  $$
  - $μ(x)$: 평균값으로, $x$의 절댓값이 커질수록 작아짐 → CV 증가
  - $σ(x)$: 표준편차로, $x$의 절댓값이 커질수록 작아짐 → CV 증가
  ![[Pasted image 20250923035216.png]]
- 의미: 계수 분포의 **상대적 변동성**
- CV가 크다 → 변경에 덜 민감 → 임베딩, 즉 메시지 은닉에 유리  
- CV가 작다 → 변경에 민감 → 임베딩 비용 증가, 즉 메시지 은닉에 불리

---

## 3. 일반화된 Uniform Embedding 전략
- 목표: 각 통계 모델의 bin 변화량을 **CV 비례**로 분산시킴
- 즉, 모든 계수에 동일한 확률로 임베딩하지 않고,  
  **탐지 저항성이 높은 위치**에 더 많은 변경 허용 (탐지 저항성: 단계 3 참고)
	- 기존의 스테가노그래피 알고리즘의 경우, zero AC 계수의 수를 줄이거나 새로운 AC 계수를 만드는 것을 피함 (위에서 봤지만, 빈도값 0의 평균 $\mu$가 커져 CV가 작아지기 때문)
	- UERD의 경우, zero AC 계수도 distortion function의 결과에 따라 메시지를 숨길 수 있음

---

## 4. UERD Distortion Function
UERD의 비용 함수(ρ)는 두 가지 요소로 구성됨 
	$x_{ij}$: $(m, n)$ 위치의 8 x 8 DCT 블록에서 $(i, j)$ 위치의 계수


$$
\rho_{ij} = \rho_{ij,mode} \cdot \rho_{ij,block}
$$

- **Mode 기반 비용 (ρᵢⱼ,mode)**  
  - DCT 계수의 주파수 모드별 특성 반영  
  - 정의:  
    $\rho_{ij,mode} = q_{ij}$ ($qᵢⱼ$ = 해당 계수의 JPEG 양자화 계수)

- **Block 기반 비용 (ρᵢⱼ,block)**  
  - 계수가 속한 블록 및 주변 블록의 에너지 고려  
  - 블록 에너지 정의:
    $D_{mn} = \sum_{k=0}^{7}\sum_{l=0}^{7} |x_{kl}| \cdot q_{kl}, \quad x_{00}=0$
	  - $D_{mn}$이 클수록 최종 block 비용이 작아져 복잡한 블록에 더 많이 삽입 가능

  - 최종 block 비용:
$$
    \rho_{ij,block} =
    \begin{cases}
    \dfrac{0.5 \cdot (q_{i+1,j} + q_{i,j+1})}{D_{mn} + 0.25 \cdot \sum_{d \in \hat{D}} d}, & \text{DC 계수} \\
    \dfrac{q_{ij}}{D_{mn} + 0.25 \cdot \sum_{d \in \hat{D}} d}, & \text{그 외}
    \end{cases}
    $$

    - $\hat{D}$: 해당 블록 주변 8개 블록의 에너지 집합

---

## 5. 알고리즘 동작 절차
![[Pasted image 20250923043111.png]]

### 단계 1: DCT 계수 추출
- JPEG 압축 이미지의 모든 DCT 계수(DC, zero AC, non-zero AC)를 추출

### 단계 2: 통계 분석
- 모드별 히스토그램 → 평균 μ, 표준편차 σ 계산  
- CV = $σ/μ$ 산출

### 단계 3: 비용 계산
- 각 계수에 대해  
  - Mode 기반 비용 (ρᵢⱼ,mode)  
	  - JPEG 양자화 계수는 저주파에서 작고 고주파에서 큼
	  - 즉, 저주파 모드 쪽(DC와 가까운 쪽)이 기본적으로 비용이 낮아짐
  - Block 기반 비용 (ρᵢⱼ,block)  
	  - 블록 에너지 $D_{mn}$과 주변 블록 복잡도를 고려
	  - 평탄한 블록(DC, 저주파 쪽)에서는 비용이 커져 변경이 감소
	  - 복잡도가 높은 블록(texture 영역)에서는 비용이 커져 변경이 증가
  - 최종 비용 $ρᵢⱼ$ = 두 요소의 곱

### 단계 4: 최소왜곡 임베딩
- Syndrome-Trellis Codes (STC) 이용  
- 주어진 페이로드를 만족하면서 총 비용 $Σρ$ 최소화
- 각 계수에 대해 최종 비용 $ρᵢⱼ$ 을 계산한 경우:
	- 메시지 비트 $m_i$를 은닉하려면 계수 $c_i$를 +1, -1, 또는 그대로 해서 변화
	- 수정된 DCT 계수로 JPEG 블록 재생성

### 단계 5: 결과
- 작은 계수(특히 0 주변)를 더 자주 수정  
- 통계적 변화량이 CV에 비례하도록 분산  
- 탐지 저항성 향상, 계산 복잡도 낮음

---

## 6. 알고리즘 특성
- **장점**
  - UED보다 보안성 향상 (계수의 변화량이 적어 탐지 확률 감소)
  - 계산량 J-UNIWARD 대비 1/100 수준
  - DC/zero AC도 메시지 은닉에 활용 가능
- **한계**
  - 최신 고성능 기법(J-UNIWARD 등)에 비해 일부 탐지기에선 성능 열세

---

## 7. 요약 (Pseudo-code)

```pseudo
Input: JPEG image I, payload m
Output: Stego image I'

1. Extract quantized DCT coefficients from I
2. For each mode:
      compute μ(x), σ(x) → CV(x) = σ/μ
3. For each DCT block:
      compute block energy Dmn
4. For each coefficient xij:
      compute ρij,mode = qij
      compute ρij,block using Dmn and neighbors
      set ρij = ρij,mode * ρij,block
5. Apply STC with cost ρ to embed payload m
6. Output stego image I'
```
---
## 관련 링크
[[양자화 (Quantization)]]
[[DCT|DCT 계수]]
[[MCU]]
[[F5 Algorithm]]
