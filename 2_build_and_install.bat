@echo off
cd /d "%~dp0"
echo ============================================================
echo  VocalForge: building (first build takes ~10-15 minutes)
echo ============================================================
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 goto :fail
cmake --build build --config Release --parallel
if errorlevel 1 goto :fail

echo Copying plug-in to "%CommonProgramFiles%\VST3" ...
robocopy "build\VocalForge_artefacts\Release\VST3" "%CommonProgramFiles%\VST3" /E
if errorlevel 8 goto :copyfail

echo.
echo SUCCESS. In FL Studio: Options ^> Manage plugins ^> Find installed plugins.
pause
exit /b 0

:copyfail
echo.
echo Copy failed. Run this script as Administrator, or copy the folder
echo   build\VocalForge_artefacts\Release\VST3\VocalForge.vst3
echo to  C:\Program Files\Common Files\VST3\  manually.
pause
exit /b 1

:fail
echo.
echo BUILD FAILED. Copy the red error text above and send it to Claude.
pause
exit /b 1
