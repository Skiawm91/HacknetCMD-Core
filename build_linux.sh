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