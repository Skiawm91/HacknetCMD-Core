#!/bin/bash
echo Starting Build...
cd "$(dirname "$0")"
rm -rf build
mkdir build
cp -r assets build/
clang++ $(find source -name '*.cpp') $(find lib -name '*_linux.a') -Iinclude \
    -std=c++20 -Wdeprecated-declarations -o build/HacknetCMD
if [ -d "data" ]; then
    echo "Data detected, putting to build..."
    cp -r data build/
fi
# 生成啟動腳本
cat > build/launch.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
if ! [ -t 0 ]; then
    x-terminal-emulator -e bash -c '"$(dirname "$0")/HacknetCMD"; read'
else
    ./HacknetCMD
fi
EOF
chmod +x build/launch.sh
echo
echo "Done."
read -p "Press Enter to Run Application."
cd build
./HacknetCMD
read -p "Do you want to pack the build? (y/N) " Do
if [[ "$Do" == "y" || "$Do" == "Y" ]]; then
    read -p "Enter version: " ver
    7zz a -t7z HacknetCMD_${ver}-linux.7z * -x!'data' -mx=9 -m0=lzma2 -mmt=on
fi
echo "Done."