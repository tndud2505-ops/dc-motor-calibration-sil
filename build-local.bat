@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"
where wsl.exe >nul 2>nul
if not errorlevel 1 (
    echo [BUILD] WSL compiler: cc
    echo [RUN] WSL dc_motor_sil
    wsl.exe --cd "%~dp0" sh -lc "set -e; cc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code/provided_control.c code/sil_environment.c code/main.c -o dc_motor_sil; ./dc_motor_sil"
    set "BUILD_RESULT=!errorlevel!"
    goto :finish
)
where gcc >nul 2>nul
if not errorlevel 1 (
    echo [BUILD] Native compiler: gcc
    gcc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code\provided_control.c code\sil_environment.c code\main.c -o dc_motor_sil.exe
    if not errorlevel 1 (
        echo [RUN] dc_motor_sil.exe
        dc_motor_sil.exe
        set "BUILD_RESULT=!errorlevel!"
        goto :finish
    )
    echo [WARN] Native GCC failed. Trying the next available compiler.
)
where clang >nul 2>nul
if not errorlevel 1 (
    echo [BUILD] Native compiler: clang
    clang -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Icode code\provided_control.c code\sil_environment.c code\main.c -o dc_motor_sil.exe
    if errorlevel 1 goto :build_failed
    echo [RUN] dc_motor_sil.exe
    dc_motor_sil.exe
    set "BUILD_RESULT=!errorlevel!"
    goto :finish
)
echo [ERROR] No C compiler was found. Install GCC, Clang, or WSL with build-essential. See LOCAL_BUILD.md.
set "BUILD_RESULT=1"
goto :finish

:build_failed
echo [ERROR] C compilation failed.
set "BUILD_RESULT=1"

:finish
if /I not "%~1"=="--no-pause" pause
exit /b %BUILD_RESULT%
