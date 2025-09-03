@echo off
echo ========================================
echo     ESP32 Projekt Auto-Backup
echo ========================================
echo.

cd /d "%~dp0"
powershell -ExecutionPolicy Bypass -File auto-backup.ps1

echo.
echo ========================================
echo Druecke eine Taste zum Beenden...
pause >nul
