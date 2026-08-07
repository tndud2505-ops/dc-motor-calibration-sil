# DC Motor SIL 환경 구현 과제

## 목표

사이트에서 제공한 제어 코드는 그대로 사용하고, PC에서도 제어 코드가 동작하도록 가상의 모터 환경을 구현합니다.

학생이 수정할 파일은 `sil_environment.c` 하나뿐입니다. 나머지 파일은 제공 코드이므로 수정하지 않습니다.

실제 장비의 전류와 Stopper 조건은 수강생마다 동일하게 검증하기 어렵습니다. 따라서 이번 과제에서는 정밀한 모터 물리 모델보다 SIL의 기본 구조, 로컬 C 빌드, GitHub Push와 자동 빌드 경험을 우선합니다.

## 과제 진행 순서

1. GitHub 계정 없이 ZIP 파일을 내려받습니다.
2. [LOCAL_BUILD.md](LOCAL_BUILD.md)에 따라 제공 예제를 로컬에서 빌드합니다.
3. [SIL_GUIDE.md](SIL_GUIDE.md)를 읽고 `sil_environment.c`의 역할을 이해합니다.
4. SIL 환경을 수정한 뒤 로컬에서 다시 빌드합니다.
5. 마지막에 [GIT_SUBMISSION.md](GIT_SUBMISSION.md)를 따라 GitHub 계정을 만들고 학생 브랜치에 Push합니다.

## `sil_environment.c`에서 구현할 내용

1. 가상 모터 위치
   - 이동 범위는 LOWER `0`부터 UPPER `100`까지로 둡니다.
   - 초기 물리 위치는 `50`입니다.
   - CW 명령이면 한 tick마다 위치가 `+1` 됩니다.
   - CCW 명령이면 한 tick마다 위치가 `-1` 됩니다.

2. Hall 파형과 인터럽트
   - 물리 위치가 1만큼 이동할 때마다 Hall pulse 1개가 발생한 것으로 봅니다.
   - Hall pulse가 발생하면 `EXTI6_IRQHandler()`를 한 번 호출합니다.

3. 전류와 ADC 인터럽트
   - 정상 이동 중 전류값은 `1500`입니다.
   - 모터가 LOWER 또는 UPPER 끝단을 계속 밀면 Stall 전류값 `3200`이 됩니다.
   - 정지 상태의 전류값은 `0`입니다.
   - 매 tick마다 전류값을 갱신한 뒤 `ADC1_2_IRQHandler()`를 호출합니다.

4. 모터 명령 함수
   - `SIL_MotorCW()`는 CW 이동 상태를 저장합니다.
   - `SIL_MotorCCW()`는 CCW 이동 상태를 저장합니다.
   - `SIL_MotorStop()`은 이동을 정지합니다.

## 제공 코드의 역할

- `provided_control.c`: CAL, GET ON 50%, GET OFF 10%, STOP 동작을 결정합니다.
- `main.c`: 명령을 순서대로 실행하고 상태 변수만 출력합니다.
- `sil_api.h`: 제공 코드와 SIL 환경 사이의 함수 규약입니다.

실제 MCU의 GPIO, TIM, ADC, UART 레지스터는 이 과제에서 구현하지 않습니다. SIL에서는 레지스터 대신 위 함수 호출과 인터럽트 호출로 실제 환경을 모사합니다.

## 제출

1. 로컬에서 `build-local.bat` 또는 `sh build-local.sh`를 실행합니다.
2. GitHub 저장소에 `student/학번` 브랜치를 만듭니다.
3. 변경한 `sil_environment.c`를 커밋하고 Push합니다.
4. GitHub Actions의 `Build and run SIL` 로그를 확인합니다.

GitHub Actions의 목적은 자동 채점이 아니라 코드가 PC에서 빌드되고 끝까지 실행되는지 확인하는 것입니다.
