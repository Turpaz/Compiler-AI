@echo off
set COMPILER=.\build\bin\compiler.exe
set RUNTIME=src\Runtime\runtime.cpp
set CLANG=C:\msys64\mingw64\bin\clang++.exe

if "%1"=="" (
    echo Usage: compile_and_run.bat ^<source_file.lang^>
    exit /b 1
)

echo [1/3] Compiling %1 to output.ll...
%COMPILER% %1 output.ll
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    exit /b 1
)

echo [2/3] Assembling and Linking with Runtime...
%CLANG% output.ll %RUNTIME% -o program.exe -Wno-override-module
if %ERRORLEVEL% NEQ 0 (
    echo Linking failed!
    exit /b 1
)

echo [3/3] Running program.exe...
echo ========================================
program.exe
echo ========================================
