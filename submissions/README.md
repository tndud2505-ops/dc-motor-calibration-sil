# 학생 제출 폴더

학생별 폴더를 만들고 제어 로직 파일만 추가합니다.

```text
submissions/
└── <학생ID>/
    └── control_logic.c
```

`control_logic.c`는 `sil/dc_motor_control.h`의 인터페이스를 구현해야 합니다.
MCU 레지스터를 직접 다루지 않고, `dc_motor_platform_t` 콜백과 Hall pulse를
사용해 CALIBRATION, GET ON, GET OFF, STOP 동작을 구현합니다.

로컬 검증:

```text
sil/run-sil.bat submissions/<학생ID>/control_logic.c
```

GitHub에 Pull Request를 올리면 Actions가 같은 SIL 검증을 자동으로 실행합니다.
`success=1`로 종료되고 `calibration_test`, `get_off_test`, `get_on_test`,
`stop_test`가 모두 `1`이어야 통과입니다.
