@echo off
if "%1"=="" (
    echo Usage: test.bat ^<source_file.lang^>
    exit /b 1
)
.\build\bin\compiler.exe %1
