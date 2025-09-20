@echo off
setlocal enabledelayedexpansion
echo Starting Build...
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set "VSPath=%%i"
)
if not defined VSPath (
    echo [Error] Not exist Visual Studio, Please install vswhere or Visual Studio.
    pause
    exit
)
call "%VSPath%\Common7\Tools\VsDevCmd.bat"
rmdir /S /Q build > nul
mkdir build build\assets > nul
xcopy /E assets build\assets > nul
set files=
for /r "source" %%f in (*.cpp) do (
    set files=!files! "%%f"
)
cl /EHsc /nologo /std:c++20 /utf-8 /FeBuild\\HacknetCMD.exe /FoBuild\ !files! /link icon.res advapi32.lib winmm.lib user32.lib windowsapp.lib
if exist build\*.obj (
    del /F /Q build\*.obj
)
echo.
echo Done.
echo Press Enter to Run Application.
pause > nul
cd build
start HacknetCMD.exe