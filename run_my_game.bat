@echo off
set FILE_NAME=main
powershell -ExecutionPolicy Bypass -File "src\setup.ps1" -fileName "%FILE_NAME%.exe"
pause
exit
