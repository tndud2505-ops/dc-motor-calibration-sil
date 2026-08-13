# `build-local.bat` 줄별 설명

`build-local.bat`에는 실행에 필요한 명령만 둡니다. 아래 코드의 `//` 줄은 실제 배치 파일에 들어가는 주석이 아니라, 각 명령의 역할을 설명하는 문서용 주석입니다.

## 시작 설정

```text
@echo off
// 배치 명령 자체가 화면에 반복해서 출력되지 않도록 합니다.

setlocal EnableExtensions EnableDelayedExpansion
// 이 배치 파일 안에서만 환경 변수 설정을 사용합니다.
// EnableExtensions는 배치 파일 확장 기능을 켭니다.
// EnableDelayedExpansion은 괄호 블록 안에서 !변수! 값을 최신 상태로 읽게 합니다.

cd /d "%~dp0"
// %~dp0는 이 배치 파일이 있는 폴더의 드라이브와 경로입니다.
// /d 옵션은 다른 드라이브에 있어도 해당 폴더로 이동하게 합니다.
```

## WSL 컴파일러 확인 및 실행

```text
where wsl.exe >nul 2>nul
// WSL 실행 파일이 설치되어 있는지 확인합니다.
// 정상 출력과 오류 메시지는 화면에 표시하지 않습니다.

if not errorlevel 1 (
// 바로 앞의 where 명령이 성공했으면 errorlevel은 0입니다.
// WSL이 있으면 Windows GCC와 Clang보다 WSL의 cc를 먼저 사용합니다.

    echo [BUILD] WSL compiler: cc
    // 현재 사용할 컴파일러가 WSL의 cc임을 표시합니다.

    echo [RUN] WSL dc_motor_sil
    // WSL에서 실행할 프로그램을 화면에 표시합니다.

    wsl.exe --cd "%~dp0" sh -lc "set -e; cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code/provided_control.c code/sil_environment.c code/main.c -o dc_motor_sil; ./dc_motor_sil"
    // --cd는 WSL의 작업 폴더를 현재 프로젝트 폴더로 설정합니다.
    // sh -lc는 WSL 안에서 명령 문자열을 실행합니다.
    // set -e는 컴파일이 실패하면 실행 단계로 넘어가지 않게 합니다.
    // -std=c11은 C11 표준으로 컴파일합니다.
    // -O2는 최적화 수준을 설정합니다.
    // -Wall과 -Wextra는 경고를 폭넓게 표시합니다.
    // -Werror는 경고도 오류로 처리합니다.
    // -pedantic은 표준에서 벗어난 문법을 엄격하게 검사합니다.
    // -Icode는 헤더 파일을 code 폴더에서 찾도록 합니다.
    // 세 C 파일을 하나의 dc_motor_sil 실행 파일로 빌드한 뒤 실행합니다.

    set "BUILD_RESULT=!errorlevel!"
    // WSL 컴파일 또는 프로그램 실행 결과를 저장합니다.
    // 괄호 블록 안에서도 최신 결과를 읽기 위해 !errorlevel!을 사용합니다.

    goto :finish
    // 다른 컴파일러 검사로 가지 않고 종료 처리로 이동합니다.
)
```

## Windows GCC 확인 및 실행

```text
where gcc >nul 2>nul
// Windows용 GCC가 설치되어 있는지 확인합니다.

if not errorlevel 1 (
// GCC가 발견되었을 때만 Windows 네이티브 빌드를 시도합니다.

    echo [BUILD] Native compiler: gcc
    // 현재 사용할 컴파일러가 Windows GCC임을 표시합니다.

    gcc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code\provided_control.c code\sil_environment.c code\main.c -o dc_motor_sil.exe
    // 제공 코드와 학생 구현 코드를 Windows 실행 파일로 컴파일합니다.
    // Windows 배치 파일에서는 폴더 구분자로 백슬래시를 사용합니다.

    if not errorlevel 1 (
    // GCC 컴파일이 성공했을 때만 실행 파일을 실행합니다.

        echo [RUN] dc_motor_sil.exe
        // 실행할 Windows 실행 파일을 화면에 표시합니다.

        dc_motor_sil.exe
        // 방금 빌드한 프로그램을 실행합니다.

        set "BUILD_RESULT=!errorlevel!"
        // 프로그램의 종료 코드를 저장합니다.

        goto :finish
        // 실행 결과를 유지한 채 종료 처리로 이동합니다.
    )

    echo [WARN] Native GCC failed. Trying the next available compiler.
    // GCC가 설치되어 있어도 컴파일에 실패할 수 있으므로 Clang 검사를 계속합니다.
)
```

## Windows Clang 확인 및 실행

```text
where clang >nul 2>nul
// Windows용 Clang이 설치되어 있는지 확인합니다.

if not errorlevel 1 (
// Clang이 발견되었을 때만 Clang 빌드를 시도합니다.

    echo [BUILD] Native compiler: clang
    // 현재 사용할 컴파일러가 Windows Clang임을 표시합니다.

    clang -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code\provided_control.c code\sil_environment.c code\main.c -o dc_motor_sil.exe
    // GCC와 같은 옵션으로 세 C 파일을 컴파일합니다.

    if errorlevel 1 goto :build_failed
    // Clang 컴파일에 실패하면 실패 처리로 이동합니다.

    echo [RUN] dc_motor_sil.exe
    // 실행할 Windows 실행 파일을 화면에 표시합니다.

    dc_motor_sil.exe
    // Clang으로 만든 실행 파일을 실행합니다.

    set "BUILD_RESULT=!errorlevel!"
    // 프로그램의 종료 코드를 저장합니다.

    goto :finish
    // 실행 결과를 유지한 채 종료 처리로 이동합니다.
)
```

## 컴파일러가 없거나 빌드에 실패한 경우

```text
echo [ERROR] No C compiler was found. Install GCC, Clang, or WSL with build-essential. See LOCAL_BUILD.md.
// 사용할 수 있는 컴파일러가 하나도 없을 때 오류 메시지를 표시합니다.

set "BUILD_RESULT=1"
// 컴파일러가 없었다는 실패 코드 1을 저장합니다.

goto :finish
// 공통 종료 처리로 이동합니다.

:build_failed
// 컴파일 명령이 실패했을 때 도착하는 레이블입니다.

echo [ERROR] C compilation failed.
// C 컴파일 실패 메시지를 표시합니다.

set "BUILD_RESULT=1"
// 컴파일 실패를 나타내는 종료 코드 1을 저장합니다.
```

## 종료 처리

```text
:finish
// WSL, GCC, Clang 실행이 끝난 뒤 공통으로 사용하는 종료 지점입니다.

if /I not "%~1"=="--no-pause" pause
// --no-pause 인자를 주지 않았으면 창이 바로 닫히지 않도록 멈춥니다.
// 사용자가 키를 누르면 배치 파일이 종료됩니다.

exit /b %BUILD_RESULT%
// 배치 파일을 호출한 프로그램에 최종 결과 코드를 반환합니다.
```

## 컴파일러 선택 순서

```text
WSL의 cc → Windows GCC → Windows Clang
```

앞 단계의 컴파일러가 있으면 해당 분기를 사용합니다. WSL이 설치되어 있으면 WSL 명령의 성공 여부와 관계없이 `:finish`로 이동하므로 Windows GCC와 Clang은 확인하지 않습니다. WSL이 없을 때만 Windows GCC를 확인하고, GCC 빌드가 실패하면 Clang을 확인합니다.
