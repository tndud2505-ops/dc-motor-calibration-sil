@echo off
setlocal
set "CONTROL_SOURCE=%~1"
if "%CONTROL_SOURCE%"=="" set "CONTROL_SOURCE=sil\dc_motor_control.c"
pushd "%~dp0.."
where make >nul 2>nul
if %errorlevel% equ 0 (
  make sil CONTROL_SOURCE="%CONTROL_SOURCE:/=\%"
) else (
  where gcc >nul 2>nul
  if not %errorlevel% equ 0 (
    echo gcc or make is required to run the SIL test.
    popd
    exit /b 1
  )
  gcc -std=c11 -O2 -Wall -Wextra -Werror -pedantic -Isil "%CONTROL_SOURCE%" sil\sil_environment.c sil\sil_main.c -o sil\dc_motor_sil.exe
  if not %errorlevel% equ 0 exit /b %errorlevel%
  sil\dc_motor_sil.exe
)
set result=%errorlevel%
popd
exit /b %result%
