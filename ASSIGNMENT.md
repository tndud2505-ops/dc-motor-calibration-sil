# DC Motor IMS 제어 로직 및 SIL 검증 과제

## 1. 구현해야 하는 사양

- 명령은 `0x01 GET ON`, `0x02 GET OFF`, `0x03 STOP`, `0x04 CALIBRATION`입니다.
- CAL은 CW로 UPPER 끝단을 찾고, 이어서 CCW로 LOWER 끝단을 찾습니다.
- CAL 완료 후 좌표는 `LOWER=0`, `UPPER=전체 Hall stroke`입니다.
- CW Hall pulse는 위치 `+1`, CCW Hall pulse는 위치 `-1`입니다.
- GET ON은 전체 stroke의 50%에서 정지합니다.
- GET OFF는 LOWER 기준 전체 stroke의 10%에서 정지합니다.
- STOP은 CAL 또는 위치 이동을 즉시 중단합니다.

목표 위치 계산식은 다음과 같습니다.

```text
target = LOWER + (UPPER - LOWER) * percent / 100
```

## 2. 공통 코드와 환경 코드

### common: 학생 구현 및 제출 영역

`common/dc_motor_control.c/.h`와 같은 역할입니다.

- UART에서 전달된 CMD/DATA의 의미를 판단합니다.
- CAL과 위치 이동 상태를 전환합니다.
- Hall pulse 수를 이용해 현재 위치를 계산합니다.
- GET ON 50%, GET OFF 10% 목표를 계산합니다.
- 필요한 방향의 `motor_cw`, `motor_ccw`, `motor_stop` callback을 호출합니다.
- MCU 레지스터, `training_api.h`, ADC/TIM/GPIO/USART 레지스터를 직접 사용하지 않습니다.

### target: 실제 MCU에서만 사용하는 고정 영역

`target/dc_motor_target.c`입니다.

- GPIO, TIM2 PWM, ADC1, USART1, EXTI6, Hall 레지스터를 설정합니다.
- ADC 전류로 Stopper 접촉 여부를 제공합니다.
- Hall pulse 수와 UART 명령을 common 제어 로직에 전달합니다.
- 학생 제출 대상이 아닙니다.

### sil: PC 자동 검증에서만 사용하는 고정 영역

`sil/sil_environment.c/.h`, `sil/sil_main.c`입니다.

- 실제 모터 이동, Hall pulse, 양 끝 Stopper를 가상으로 만듭니다.
- CAL → GET OFF → GET ON → STOP을 자동 실행합니다.
- 실제 MCU 레지스터나 전기적 과도현상은 모사하지 않습니다.
- 학생 제출 대상이 아닙니다.

## 3. 학생 제출 파일

학생은 다음 파일 하나만 제출합니다.

```text
submissions/<학번>/control_logic.c
```

시작 파일은 `student/control_logic_template.c`입니다. 함수 원형과 callback 구조를 유지하고
TODO를 구현합니다. `target/`, `sil/`, `common/dc_motor_control.h`, `Makefile`은 수정하지 않습니다.

## 4. 통과 기준

자동 검증 결과가 모두 다음 조건을 만족해야 합니다.

- CAL 완료: `LOWER=0`, `UPPER=100`
- GET OFF: 현재 위치와 목표 위치가 `10`
- GET ON: 현재 위치와 목표 위치가 `50`
- 이동 중 STOP: 즉시 `IDLE`, 방향 `STOP`
- 출력: `calibration_test=1`, `get_off_test=1`, `get_on_test=1`, `stop_test=1`, `success=1`
- 프로세스 종료 코드: `0`

## 5. 실행과 제출

```text
sil/run-sil.bat submissions/<학번>/control_logic.c
```

또는 Linux/macOS/Git Bash에서는 다음을 사용합니다.

```text
CONTROL_SOURCE=submissions/<학번>/control_logic.c sil/run-sil.sh
```

검증을 통과한 `control_logic.c`만 학생 브랜치에 커밋하고 Pull Request로 제출합니다.
