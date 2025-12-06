@echo off
echo Cleaning and rebuilding compiler...
if exist build rmdir /s /q build
if exist CMakeCache.txt del /q CMakeCache.txt
if exist CMakeFiles rmdir /s /q CMakeFiles
cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_C_COMPILER="C:/msys64/mingw64/bin/clang.exe" -DCMAKE_CXX_COMPILER="C:/msys64/mingw64/bin/clang++.exe" -DCMAKE_C_COMPILER_TARGET=x86_64-w64-windows-gnu -DCMAKE_CXX_COMPILER_TARGET=x86_64-w64-windows-gnu
cmake --build build
if %ERRORLEVEL% EQU 0 (
    echo Rebuild successful!
) else (
    echo Rebuild failed!
)
