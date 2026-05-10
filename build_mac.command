#!/bin/bash
echo Starting Build...
cd "$(dirname "$0")"
rm -rf build
mkdir build
cp -r assets build/
clang++ $(find source -name '*.cpp') $(find lib -name '*_mac.a') -Iinclude \
    -framework CoreFoundation -framework CoreServices -framework AppKit \
    -std=c++20 -Wdeprecated-declarations -mmacosx-version-min=11.0 -arch x86_64 -arch arm64 -o build/HacknetCMD
FOLDER=$1
if [ -d "data" ]; then
    echo "Data detected, putting to build..."
    cp -r data build/
fi
echo
echo "Done."
read -p "Press Enter to Run Application."
cd build
./HacknetCMD