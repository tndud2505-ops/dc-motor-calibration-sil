# DC Motor CALIBRATION SIL 과제

공유 저장소: https://github.com/tndud2505-ops/dc-motor-calibration-sil

이 과제에서 학생은 실제 MCU 코드를 다시 작성하지 않습니다. 제공된 제어 로직을 PC에서 실행할 수 있도록 `sil_environment.c` 하나만 구현합니다.

## 파일 구조

```text
provided_control.c   수정 금지: 제공되는 모터 제어 로직
sil_environment.c   학생 작성: 모터·전류·Hall·인터럽트 모사
main.c              수정 금지: 명령 실행 및 변수 출력
sil_api.h           수정 금지: 공용 함수와 상수 선언
Makefile            빌드 및 실행
ASSIGNMENT.md        상세 과제 설명
```

## 제어 사양

- `0x01 GET ON`: LOWER 기준 50% 위치로 이동
- `0x02 GET OFF`: LOWER 기준 10% 위치로 이동
- `0x03 STOP`: 즉시 정지
- `0x04 CALIBRATION`: CW로 UPPER를 찾은 뒤 CCW로 LOWER를 찾음
- CAL 완료 좌표: `LOWER=0`, `UPPER=전체 Hall pulse 수`
- CW Hall pulse: 현재 위치 `+1`
- CCW Hall pulse: 현재 위치 `-1`

## 실행

```bash
make run
```

`main.c`는 CALIBRATION, GET OFF, GET ON, STOP 순서로 명령을 실행하고 변수만 출력합니다. 별도의 정답 판정 코드는 포함하지 않습니다.

학생은 자신의 GitHub 브랜치에서 `sil_environment.c`만 수정해 커밋하고 Pull Request를 생성합니다. GitHub Actions는 같은 명령으로 빌드와 실행 가능 여부를 확인합니다.
