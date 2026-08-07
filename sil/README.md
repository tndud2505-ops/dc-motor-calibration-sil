# sil: PC 가상 검증 환경

이 폴더는 실제 MCU 대신 PC에서 `common/` 제어 로직을 검증합니다.

- `sil_environment.c`: 모터 위치, Hall pulse, LOWER/UPPER Stopper를 모사합니다.
- `sil_main.c`: CAL, GET OFF 10%, GET ON 50%, STOP을 자동 판정합니다.
- 실제 레지스터와 전기적 과도현상은 모사하지 않습니다.
- 학생 제출 대상이 아닙니다.
