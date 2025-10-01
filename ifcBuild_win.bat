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
rmdir /S /Q interface > nul
mkdir interface > nul
for /r "source" %%F in (*.cppm) do (
    set "name=%%~nF"
    set "folder=%%~dpF"
    set "folder=!folder:~7!"  :: 假設 source\ 是 7 個字元，視你的路徑調整
    if "!folder:~-1!"=="\" set "folder=!folder:~0,-1!"
    set "last="
    for %%A in ("!folder!") do set "last=%%~nxA"
    if "!last!"=="source" (
        set "ifc=interface\!name!.ifc"
    ) else (
        set "ifc=interface\!last!.!name!.ifc"
    )
    echo !last!\!name!.cppm
    cl /c /EHsc /nologo /std:c++20 /utf-8 /interface /TP "%%F" /Fo!ifc! | findstr /V "!name!.cppm"
)
echo.
echo Done.
echo Press Enter to Exit.
pause > nul
exit