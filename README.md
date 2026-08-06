# DC Motor CALIBRATION SIL 과제

이 저장소는 DC 모터 과제의 최종 구조를 MCU 코드와 호스트 SIL 코드로 나누어
검증하기 위한 공개 예제입니다.

## 과제 정의

- 초기 위치는 `1000`입니다.
- CCW 방향으로 Hall pulse가 들어오면 현재 위치를 `+1` 합니다.
- CW 방향으로 Hall pulse가 들어오면 현재 위치를 `-1` 합니다.
- `CALIBRATION`은 CCW로 END POINT를 찾고, CW로 START POINT를 찾습니다.
- `GET_ON`은 50%, `GET_OFF`는 10% 위치로 이동합니다.
- 목표 위치는 `START + (END - START) * percent / 100`입니다.

## 영역 분리

| 영역 | 위치 | 역할 |
| --- | --- | --- |
| 펌웨어/하드웨어 | `reference/dc_motor_reference.c`의 `FW_*`, `user_init` | 레지스터, PWM, UART, ADC, EXTI, Hall 직접 제어 |
| 제어 로직 | `reference/dc_motor_reference.c`의 `CL_*` | 명령 해석, 상태 전이, 위치 계산 |
| SIL 환경 | `sil/sil_environment.c` | 모터·Hall·Stopper를 가상으로 대체 |
| 학생 제출 | `student/`, `submissions/` | 제어 로직 구현 및 PR 제출 |

## 로컬 SIL 실행

```text
sil/run-sil.bat       # Windows
sil/run-sil.sh        # Linux/macOS/Git Bash
```

테스트 프로그램은 `CALIBRATION` 다음 `GET_OFF`를 실행합니다. 출력은 상태 변수만
보여주며, `success=1`이고 종료 코드가 `0`이면 통과입니다. 학생 코드로 바꿔
검증하려면 다음처럼 실행합니다.

```text
sil/run-sil.bat submissions/1234/control_logic.c
CONTROL_SOURCE=submissions/1234/control_logic.c sil/run-sil.sh
```

## 사이트 예제

현재 Virtual MCU 사이트의 `DC Motor` 프로젝트에서 `FW EDIT`를 열고
`CALIBRATION 완성 예제`를 선택할 수 있습니다. UART에서 다음 순서로 실행합니다.

```text
CALIBRATION (0x04, DATA 0x00) -> 완료 대기 -> GET_OFF (0x02, DATA 0x00)
```

사이트 Watch에서 `current_position`, `start_point`, `end_point`,
`target_position`, `controller_state`, `calibration_complete`를 확인합니다.
