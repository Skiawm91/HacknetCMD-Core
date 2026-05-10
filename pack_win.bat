@echo off
cd build
set /p ver=Enter version: 
7z a -t7z HacknetCMD_%ver%-win.7z * -x!'data' -mx=9 -m0=lzma2 -mmt=on
echo Done.