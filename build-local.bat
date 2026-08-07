@echo off
setlocal

cd /d "%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-local.ps1"
set "BUILD_RESULT=%errorlevel%"

if /I not "%~1"=="--no-pause" pause
exit /b %BUILD_RESULT%
