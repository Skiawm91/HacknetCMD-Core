#define _HAS_STD_BYTE 0
#include "console.h"
#include "data.h"
#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <iostream>
#endif
using namespace std;

extern Data dta;

void Console::resize(const int width, const int height) {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    int curX = csbi.dwSize.X;
    int curY = csbi.dwSize.Y;
    bool enlarge = (width > curX) || (height > curY);
    COORD bufferSize = {(SHORT)width, (SHORT)height};
    SMALL_RECT windowSize = {0, 0, (SHORT)(width - 1), (SHORT)(height - 1)};

    if (dta.cfg.vt100Resize) {
        // --- 模式 A：VT100 模式 (Windows Terminal) ---
        if (enlarge) {
            // 放大時：先放大緩衝區，再發送 VT 放大視窗
            SetConsoleScreenBufferSize(hOut, bufferSize);
            print("\033[8;" + to_string(height) + ";" + to_string(width) + "t");
        } else {
            // 縮小時：必須先發送 VT 序列讓視窗縮小
            print("\033[8;" + to_string(height) + ";" + to_string(width) + "t");
            
            // 為了讓 Windows Terminal 有時間反應，或強行讓 Buffer 縮小
            // 如果 WinAPI 直接改 Buffer 失敗，可以加上這行 WinAPI 視窗同步輔助
            SetConsoleWindowInfo(hOut, TRUE, &windowSize);
            SetConsoleScreenBufferSize(hOut, bufferSize);
        }
    } else {
        // --- 模式 B：傳統 Conhost 模式 (純 WinAPI) ---
        if (enlarge) {
            SetConsoleScreenBufferSize(hOut, bufferSize);
            SetConsoleWindowInfo(hOut, TRUE, &windowSize);
        } else {
            SetConsoleWindowInfo(hOut, TRUE, &windowSize);
            SetConsoleScreenBufferSize(hOut, bufferSize);
        }
    }
    #elif defined(__APPLE__) || defined(__linux__)
    print("\033[8;" + to_string(height) + ";" + to_string(width) + "t");
    #endif
}