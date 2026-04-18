@echo off
echo EVolocity Arduino Setup
echo =======================
echo.
echo This will install the required libraries into your Arduino IDE.
echo.

powershell -ExecutionPolicy Bypass -File "%~dp0install-dependencies.ps1"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Setup failed. Please ask your teacher for help.
    pause
    exit /b 1
)

echo.
echo Setup complete^^! Open the Arduino IDE and you are ready to go.
pause
