# common: 공통 제어 로직(ASW)

이 폴더의 코드는 실제 MCU와 SIL에서 동일하게 사용합니다.

- 명령 해석, 상태 전이, CAL 순서, 위치와 목표 계산만 담당합니다.
- 하드웨어 동작은 `dc_motor_platform_t` callback으로 요청합니다.
- `training_api.h`와 MCU 레지스터를 사용하지 않습니다.
- 학생 제출 코드는 이 계층의 `dc_motor_control.c`를 대체합니다.
