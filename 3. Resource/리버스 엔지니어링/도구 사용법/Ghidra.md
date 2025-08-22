---
tags:
  - Dashboard
---


# 🛠️ Ghidra란?

**Ghidra**는 미국 국가안보국(NSA)에서 개발한 **무료 오픈소스 리버스 엔지니어링 도구**입니다. 악성코드 분석, 취약점 리서치, 바이너리 디핑 등 다양한 보안 분야에서 활용됩니다.

- 개발사: NSA (National Security Agency)
- 라이선스: 오픈소스 (Apache License 2.0)
- 플랫폼: Windows, macOS, Linux
- 최신 버전: 11.02 (2025년 기준)

---

# 🚀 주요 특징

## ✅ 멀티 플랫폼 지원
- Java 기반으로 운영체제에 관계없이 실행 가능

## ✅ 정적 및 동적 분석
- 바이너리 파일의 구조 분석
- [[GDB]]와 연동하여 동적 디버깅 가능

## ✅ 다양한 파일 포맷 지원
- ELF, PE, Mach-O 등 다양한 실행 파일 포맷 분석 가능

## ✅ 협업 기능
- 여러 사용자가 동시에 분석 가능
- 실시간 변경 사항 공유, 히스토리 관리, 권한 설정 등

## ✅ 스크립팅 및 자동화
- Python 및 Java로 스크립트 작성 가능
- 반복 작업 자동화 및 분석 효율 향상

---

# 🔍 주요 기능

| 기능 | 설명 |
|------|------|
| 코드 브라우저 | 함수, 변수, 흐름 등을 시각적으로 분석 |
| 바이너리 패치 | 소스 코드 없이 바이너리 직접 수정 가능 |
| 디버거 | GDB 연동으로 동적 디버깅 수행 |
| 스크립팅 | 분석 자동화 및 커스텀 기능 구현 |
| 협업 분석 | 팀 단위 리버스 엔지니어링 지원 |
| 바이너리 디핑 | BinDiff와 연동하여 버전 간 차이 분석 |
| 기호 실행 | angr와 연동하여 조건 기반 실행 경로 분석 |

---

# 📦 설치 방법

1. [공식 GitHub 저장소](https://github.com/NationalSecurityAgency/ghidra)에서 최신 버전 다운로드
2. Java 11 이상 설치 필요
3. 압축 해제 후 `ghidraRun` 실행

---

# 🎯 활용 예시

- 악성코드 분석: 의심스러운 실행 파일의 동작 파악
- 취약점 분석: 함수 흐름 및 입력 처리 방식 분석
- 버그 바운티: 크랙미 문제 해결 및 취약점 리포트
- 교육 및 연구: 리버스 엔지니어링 학습용 도구로 활용

---

# 📚 참고 자료

- [Dreamhack Ghidra 강의](https://dreamhack.io/lecture/paths/reverse-engineering-ghidra)
- [벨로그 사용법 정리](https://velog.io/@sdfghjk147/Ghidra-%EC%82%AC%EC%9A%A9%EB%B2%95)
- [HSPACE 튜토리얼](https://blog.hspace.io/posts/Ghidra-tutorial-for-reversing-beginners/)
