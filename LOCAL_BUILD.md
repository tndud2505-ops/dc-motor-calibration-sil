# GitHub 가입 전 로컬 빌드 가이드

이 단계의 목표는 Git 사용이 아닙니다. 먼저 제공된 C 코드가 자신의 PC에서 컴파일되고 실행되는 경험을 만드는 것이 목표입니다.

Docker는 사용하지 않습니다. C 컴파일러 하나만 준비하면 됩니다.

## 1. 과제 파일 받기

GitHub 계정이 없어도 공개 저장소의 ZIP 파일을 받을 수 있습니다.

1. https://github.com/tndud2505-ops/dc-motor-calibration-sil 접속
2. `Code` 버튼 선택
3. `Download ZIP` 선택
4. ZIP 압축 해제

압축을 해제한 폴더에는 최소한 다음 파일이 있어야 합니다.

```text
provided_control.c
sil_environment.c
main.c
sil_api.h
build-local.bat
build-local.ps1
build-local.sh
```

## 2. Windows에서 컴파일러 준비

`build-local.bat`은 다음 순서로 컴파일러를 찾습니다.

1. Windows용 `gcc`
2. Windows용 `clang`
3. WSL 안의 `cc`

이미 GCC 또는 Clang이 설치되어 있다면 별도 준비 없이 실행할 수 있습니다.

컴파일러가 없다면 WSL 사용을 권장합니다. 관리자 권한 PowerShell에서 다음 명령을 한 번 실행하고 PC를 재시작합니다.

```powershell
wsl --install
```

설치된 Ubuntu를 처음 실행해 사용자 이름과 암호를 만든 뒤 다음 명령으로 C 컴파일러를 설치합니다.

```bash
sudo apt update
sudo apt install build-essential
```

WSL 설치에 대한 공식 설명은 [Microsoft WSL 설치 문서](https://learn.microsoft.com/windows/wsl/install)를 참고합니다.

## 3. Windows에서 빌드 및 실행

압축을 해제한 폴더에서 `build-local.bat`을 더블클릭합니다.

빌드와 실행이 끝나면 결과를 읽을 수 있도록 창이 열린 상태로 유지됩니다. 내용을 확인한 뒤 아무 키나 누르면 창이 닫힙니다.

또는 PowerShell에서 실행합니다.

```powershell
.\build-local.bat
```

이 스크립트는 다음 세 파일을 함께 컴파일합니다.

```text
provided_control.c + sil_environment.c + main.c
```

Windows GCC 또는 Clang을 찾으면 `dc_motor_sil.exe`를 만들고 실행합니다. Windows 컴파일러가 없고 WSL이 준비되어 있으면 WSL에서 `dc_motor_sil`을 만들고 실행합니다.

## 4. Linux 또는 macOS에서 빌드 및 실행

터미널에서 과제 폴더로 이동한 뒤 실행합니다.

```bash
sh build-local.sh
```

`make`가 설치되어 있다면 다음 명령도 같은 코드를 빌드합니다.

```bash
make run
```

## 5. 정상 출력 확인

핵심 출력이 다음과 같으면 제공 예제가 정상적으로 동작한 것입니다.

```text
[CALIBRATION]
calibration_complete=1
lower_point=0
upper_point=100
current_position=0

[GET OFF]
target_position=10
current_position=10

[GET ON]
target_position=50
current_position=50

[STOP]
state=0
motor_direction=0
```

`main.c`는 자동 채점을 하지 않습니다. 각 단계의 변수를 화면에 출력할 뿐입니다.

## 6. 빌드 오류 확인 방법

### `No C compiler was found`

GCC, Clang 또는 WSL의 C 컴파일러가 없습니다. 위의 WSL 설치 절차를 완료합니다.

### `cc: command not found`

WSL은 설치됐지만 C 컴파일러가 없습니다. Ubuntu에서 다음을 실행합니다.

```bash
sudo apt update
sudo apt install build-essential
```

### C 문법 오류가 출력되는 경우

오류 메시지에 표시된 파일명과 줄 번호를 확인합니다. 학생이 수정해야 할 파일은 `sil_environment.c` 하나입니다.

## 7. 로컬 빌드 다음 단계

로컬 빌드가 성공한 뒤에 GitHub 계정을 만들고 Git 사용법을 학습합니다. 처음부터 Git과 SIL을 동시에 익히지 않고 다음 순서로 진행합니다.

```text
ZIP 다운로드
→ 로컬 빌드 성공
→ SIL 환경 이해 및 수정
→ 다시 로컬 빌드
→ GitHub 계정 생성
→ 학생 브랜치에 Push
→ GitHub Actions 결과 확인
```
