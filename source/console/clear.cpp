#define _HAS_STD_BYTE 0
#include "console.h"
#include <iostream>
#include "data.h"
#ifdef _WIN32
#include <windows.h>
#endif

extern Data dta;

void Console::clear(bool clearSaved) {
    // ⭐️ 1. 清屏時，強制套用當前的 globalBgColor 塗滿全螢幕
    fillScreenBg(globalBgColor);

    // ⭐️ 2. 將游標強制歸位到左上角 (0, 0)，準備全新的繪製
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origin = { 0, 0 };
    SetConsoleCursorPosition(hOut, origin);
#else
    print("\033[H"); // VT100 模式
#endif

    // 3. 如果需要清除歷史 Buffer 記錄
    if (clearSaved) {
        savedBuffer.clear();
    }
}