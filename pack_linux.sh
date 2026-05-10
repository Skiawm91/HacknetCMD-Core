#!/bin/bash
cd "$(dirname "$0")"
cd build
read -p "Enter version: " ver
7zz a -t7z HacknetCMD_${ver}-linux.7z * -x!'data' -mx=9 -m0=lzma2 -mmt=on
echo "Done."
read -p "Press Enter to exit."