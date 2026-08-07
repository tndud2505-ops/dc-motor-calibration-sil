# DC Motor CALIBRATION SIL 과제

공유 저장소: https://github.com/tndud2505-ops/dc-motor-calibration-sil

이 과제에서 학생은 실제 MCU 코드를 다시 작성하지 않습니다. 제공된 제어 로직을 PC에서 실행할 수 있도록 `sil_environment.c` 하나만 구현합니다.

학습 순서는 다음과 같습니다.

```text
GitHub 가입 전 ZIP 다운로드와 로컬 빌드
→ SIL 환경 동작 이해 및 수정
→ 다시 로컬 빌드
→ GitHub 계정 생성과 학생 브랜치 Push
→ GitHub Actions 결과 확인
```

## 파일 구조

```text
provided_control.c   수정 금지: 제공되는 모터 제어 로직
sil_environment.c   학생 작성: 모터·전류·Hall·인터럽트 모사
main.c              수정 금지: 명령 실행 및 변수 출력
sil_api.h           수정 금지: 공용 함수와 상수 선언
Makefile            빌드 및 실행
ASSIGNMENT.md        상세 과제 설명
LOCAL_BUILD.md       GitHub 가입 전 로컬 빌드 설명
SIL_GUIDE.md         SIL 환경 함수별 상세 구현 설명
GIT_SUBMISSION.md    GitHub 계정 생성 후 브랜치 제출 설명
build-local.bat      Windows 빌드 실행 파일
build-local.ps1      Windows 컴파일러 또는 WSL 자동 선택
build-local.sh       Linux/macOS 빌드 실행 파일
```

## 제어 사양

- `0x01 GET ON`: LOWER 기준 50% 위치로 이동
- `0x02 GET OFF`: LOWER 기준 10% 위치로 이동
- `0x03 STOP`: 즉시 정지
- `0x04 CALIBRATION`: CW로 UPPER를 찾은 뒤 CCW로 LOWER를 찾음
- CAL 완료 좌표: `LOWER=0`, `UPPER=전체 Hall pulse 수`
- CW Hall pulse: 현재 위치 `+1`
- CCW Hall pulse: 현재 위치 `-1`

## GitHub 가입 전 로컬 실행

Windows에서는 압축을 해제한 폴더에서 다음 파일을 실행합니다.

```text
build-local.bat
```

Linux 또는 macOS에서는 다음 명령을 실행합니다.

```bash
sh build-local.sh
```

컴파일러 준비부터 오류 확인까지는 [LOCAL_BUILD.md](LOCAL_BUILD.md)를 참고합니다.

## Make를 사용하는 경우

```bash
make run
```

`main.c`는 CALIBRATION, GET OFF, GET ON, STOP 순서로 명령을 실행하고 변수만 출력합니다. 별도의 정답 판정 코드는 포함하지 않습니다.

`SIL_Tick()`, Hall 인터럽트, ADC 전류값을 구현하는 순서는 [SIL_GUIDE.md](SIL_GUIDE.md)를 참고합니다.

로컬 빌드가 성공한 학생은 [GIT_SUBMISSION.md](GIT_SUBMISSION.md)에 따라 GitHub 계정을 만든 뒤 자신의 `student/학번` 브랜치에서 `sil_environment.c`만 수정해 Push합니다. GitHub Actions는 같은 코드를 다시 빌드하고 실행합니다.
