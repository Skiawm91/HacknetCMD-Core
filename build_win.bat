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
call "%VSPath%\Common7\Tools\VsDevCmd.bat" -arch=amd64
rmdir /S /Q build > nul
mkdir build build\assets > nul
xcopy /E assets build\assets > nul
for /f "delims=" %%F in ('dir /b /s source\*.cpp ^| findstr /i /v "\.cppm$"') do (
    set "name=%%~nF"
    set "folder=%%~dpF"
    set "folder=!folder:~7!"  :: 假設 source\ 是 7 個字元，視你的路徑調整
    if "!folder:~-1!"=="\" set "folder=!folder:~0,-1!"
    set "last="
    for %%A in ("!folder!") do set "last=%%~nxA"
    if "!last!"=="source" (
        set "obj=build\!name!.obj"
    ) else (
        set "obj=build\!last!.!name!.obj"
    )
    echo !last!\!name!.cpp
    cl /c /EHsc /nologo /MD /I"include" /std:c++20 /utf-8 "%%F" /Fo!obj! | findstr /V "!name!.cpp"
)
echo.
echo Compiling...
link /nologo /OUT:Build\HacknetCMD.exe Build\*.obj icon.res lib\*.lib advapi32.lib winmm.lib user32.lib windowsapp.lib
echo Cleaning...
del /F /Q Build\*.obj
if exist data (
    echo Data detected, putting to build...
    mkdir build\data > nul
    xcopy /E data build\data > nul
)
echo.
echo Done.
echo Press Enter to run the application.
pause > nul
cd build
start HacknetCMD.exe