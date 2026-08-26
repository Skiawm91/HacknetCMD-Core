#define _HAS_STD_BYTE 0
#include "console.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

void Console::bufferRestore(int minY) {
    if (savedBuffer.empty()) return;

    applyFg(globalFgColor);
    applyBg(globalBgColor);

    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif

    // ⭐️ 流式水位的起跑點 (預設從選單下方，例如 minY = 2 開始)
    int streamY = minY;

    for (const auto& rec : savedBuffer) {
        applyFg(rec.fg);
        applyBg(rec.bg);

        // 1. 絕對座標 (printAt) ➔ 繪製 UI，完全不干擾 streamY
        if (rec.x != -1 && rec.y != -1) {
            #ifdef _WIN32
            COORD pos = { (SHORT)rec.x, (SHORT)rec.y };
            SetConsoleCursorPosition(hOut, pos);
            DWORD written;
            WriteConsoleA(hOut, rec.text.c_str(), (DWORD)rec.text.size(), &written, NULL);
            #else
            cout << "\033[" << (rec.y + 1) << ";" << (rec.x + 1) << "H" << rec.text;
            #endif
        } 
        // 2. 流式輸出 (println) ➔ 永遠順著 streamY 往下接
        else {
            #ifdef _WIN32
            COORD pos = { 0, (SHORT)streamY };
            SetConsoleCursorPosition(hOut, pos);
            DWORD written;
            WriteConsoleA(hOut, rec.text.c_str(), (DWORD)rec.text.size(), &written, NULL);
            #else
            cout << "\033[" << (streamY + 1) << ";1H" << rec.text;
            #endif

            // 計算字串內部的 \n，推進水位
            int lines = 0;
            for (char c : rec.text) {
                if (c == '\n') lines++;
            }
            streamY += (lines > 0 ? lines : 1);
        }
    }

    applyFg(globalFgColor);
    applyBg(globalBgColor);

    // ⭐️ 3. 重繪結束，將游標精準停留在流式水位的最新末端！
    #ifdef _WIN32
    COORD finalPos = { 0, (SHORT)streamY };
    SetConsoleCursorPosition(hOut, finalPos);
    #else
    cout << "\033[" << (streamY + 1) << ";1H" << flush;
    #endif
}