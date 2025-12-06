@echo off
echo Installing Custom Language Syntax Extension...

set EXTENSION_DIR=%USERPROFILE%\.vscode\extensions\custom-lang-syntax-1.0.0

if exist "%EXTENSION_DIR%" (
    echo Removing existing extension...
    rmdir /s /q "%EXTENSION_DIR%"
)

echo Creating extension directory...
mkdir "%EXTENSION_DIR%"

echo Copying extension files...
xcopy /E /I /Y "%~dp0*" "%EXTENSION_DIR%"

echo.
echo Installation complete!
echo Please restart Visual Studio Code to activate the extension.
echo.
pause
