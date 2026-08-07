# DC Motor SIL 환경 구현 과제

## 목표

사이트에서 제공한 제어 코드는 그대로 사용하고, PC에서도 제어 코드가 동작하도록 가상의 모터 환경을 구현합니다.

학생이 수정할 파일은 `sil_environment.c` 하나뿐입니다. 나머지 파일은 제공 코드이므로 수정하지 않습니다.

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

1. 저장소를 자신의 브랜치로 받습니다.
2. `sil_environment.c`만 작성합니다.
3. 로컬에서 `make run`을 실행합니다.
4. 변경한 `sil_environment.c`를 커밋하고 Pull Request를 생성합니다.

GitHub Actions의 목적은 자동 채점이 아니라 코드가 PC에서 빌드되고 끝까지 실행되는지 확인하는 것입니다.
