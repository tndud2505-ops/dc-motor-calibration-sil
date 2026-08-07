# 학생 제출 규칙

1. 이 저장소를 Fork합니다.
2. `student/<학번>/week4-sil` 브랜치를 만듭니다.
3. `student/control_logic_template.c`를 `submissions/<학번>/control_logic.c`로 복사합니다.
4. TODO를 구현하고 `sil/run-sil.bat submissions/<학번>/control_logic.c` 또는 `make sil CONTROL_SOURCE=submissions/<학번>/control_logic.c`를 실행합니다.
5. 변경 사항을 커밋하고 원격 저장소에 Push한 뒤 원본 저장소로 Pull Request를 엽니다.

제출물은 `common/` 영역에 해당하는 제어 로직입니다. MCU 레지스터를 직접 쓰지 않습니다.
실제 장비에서는 `target/`이, 자동 검증에서는 `sil/`이 동일한 모터·Hall·Stopper callback을
제공합니다. 학생 코드는 명령, 상태 전이, 좌표 계산만 구현합니다.

검증 성공 조건은 실행 결과의 `success=1`과 프로세스 종료 코드 `0`입니다. Pull Request에
`submissions/<학번>/control_logic.c`만 포함하면 GitHub Actions가 같은 명령으로 자동 검증합니다.
