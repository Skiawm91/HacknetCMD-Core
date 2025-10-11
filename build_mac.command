#!/bin/bash
echo Starting Build...
cd "$(dirname "$0")"
rm -rf build
mkdir build
cp -r assets build/
clang++ $(find source -name '*.cpp') -Isource/include -framework AudioToolbox -framework CoreFoundation -std=c++20 -Wdeprecated-declarations -mmacosx-version-min=11.0 -arch x86_64 -arch arm64 -Wdeprecated-declarations -o build/HacknetCMD
FOLDER=$1
if [ -d "config" ]; then
    echo "Detected config! putting to build..."
    cp -r config build/
fi
echo
echo "Done."
read -p "Press Enter to Run Application."
cd build
./HacknetCMD