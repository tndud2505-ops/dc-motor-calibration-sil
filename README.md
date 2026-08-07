# DC Motor CALIBRATION SIL 과제

공유 저장소: https://github.com/tndud2505-ops/dc-motor-calibration-sil

이 저장소는 DC 모터 과제의 최종 구조를 MCU 코드와 호스트 SIL 코드로 나누어
검증하기 위한 공개 예제입니다.

## 과제 정의

- 초기 위치는 `1000`입니다.
- IMS 좌표는 `LOWER=0`, `UPPER=전체 stroke`입니다.
- CW 방향 Hall pulse는 현재 위치를 `+1`, CCW 방향 Hall pulse는 `-1` 합니다.
- `CALIBRATION`은 CW로 UPPER를 찾고, CCW로 LOWER를 찾은 뒤 `LOWER=0`, `UPPER=stroke`로 저장합니다.
- `GET_ON`은 50%, `GET_OFF`는 10% 위치로 이동합니다.
- 목표 위치는 `LOWER + (UPPER - LOWER) * percent / 100`입니다.

## 폴더 구조와 실행 환경

```text
common/     MCU와 SIL이 함께 사용하는 제어 로직(ASW)
target/     실제 MCU 레지스터·ADC·UART·PWM·Hall 연결(FW/BSW)
sil/        PC에서 모터·Hall·Stopper를 대신하는 가상 환경과 테스트
reference/  사이트에 한 파일로 붙여 넣는 통합 정답 예제
student/    학생용 common 제어 로직 템플릿
submissions/<학번>/control_logic.c  학생 제출 파일
```

`common/`은 실제 MCU와 SIL에서 동일하게 사용합니다. MCU 레지스터를 직접 읽거나 쓰면
안 됩니다. 실제 장비에서는 `target/`이 `common/`에 callback을 제공하고, PC 검증에서는
`sil/`이 같은 callback을 제공합니다.

## 영역 분리

| 영역 | 위치 | 역할 |
| --- | --- | --- |
| 공통 제어 로직(ASW) | `common/dc_motor_control.c/.h` | 명령 해석, CAL 상태 전이, LOWER/UPPER 좌표 및 목표 계산 |
| 실제 MCU 환경(FW/BSW) | `target/dc_motor_target.c` | 레지스터, PWM, UART, ADC, EXTI, Hall 직접 제어 |
| SIL 환경 | `sil/sil_environment.c/.h` | 실제 모터·Hall·Stopper callback을 PC 가상 모델로 대체 |
| SIL 판정 | `sil/sil_main.c` | CAL, LOWER 10%, 50%, STOP 자동 판정 |
| 사이트 통합 예제 | `reference/dc_motor_reference.c` | `FW_*`와 `CL_*`를 한 파일에서 구분한 붙여넣기용 코드 |
| 학생 제출 | `student/control_logic_template.c`, `submissions/` | `common/`과 같은 API의 제어 로직만 구현 |

## 로컬 SIL 실행

```text
sil/run-sil.bat       # Windows
sil/run-sil.sh        # Linux/macOS/Git Bash
```

테스트 프로그램은 `CALIBRATION` 다음 `GET_OFF`, `GET_ON`, `STOP`을 실행합니다.
출력은 상태 변수와 테스트 결과만 보여주며, 네 테스트가 모두 `1`이고 `success=1`이며
종료 코드가 `0`이면 통과입니다. 학생 코드로 바꿔 검증하려면 다음처럼 실행합니다.

```text
sil/run-sil.bat submissions/1234/control_logic.c
CONTROL_SOURCE=submissions/1234/control_logic.c sil/run-sil.sh
```

## 사이트 예제

현재 Virtual MCU 사이트의 `DC Motor` 프로젝트에서 `FW EDIT`를 열고
`CALIBRATION 완성 예제`를 선택할 수 있습니다. UART에서 다음 순서로 실행합니다.

```text
CALIBRATION (0x04, DATA 0x00) -> 완료 대기 -> GET_OFF (0x02) -> GET_ON (0x01) -> STOP (0x03)
```

사이트 Watch에서 `current_position`, `start_point`, `end_point`,
`target_position`, `controller_state`, `calibration_complete`를 확인합니다.

사이트용 정답은 `reference/dc_motor_reference.c`입니다. 실제 프로젝트 구조의 정답은
`common/dc_motor_control.c`와 `target/dc_motor_target.c`로 분리되어 있습니다.
GitHub Actions는 실제 레지스터 파일인 `target/`을 제외하고, `common/` 또는 학생의
`control_logic.c`를 `sil/`과 연결하여 자동 빌드·검증합니다.

자세한 과제 범위와 제출 기준은 `ASSIGNMENT.md`를 확인합니다.
