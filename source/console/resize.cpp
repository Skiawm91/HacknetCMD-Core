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
    // ... Windows 原本的程式碼保持不變 ...
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    int curX = csbi.dwSize.X;
    int curY = csbi.dwSize.Y;
    bool enlarge = (width > curX) || (height > curY);
    COORD bufferSize = {(SHORT)width, (SHORT)height};
    SMALL_RECT windowSize = {0, 0, (SHORT)(width - 1), (SHORT)(height - 1)};

    if (dta.cfg.vt100Resize) {
        if (enlarge) {
            SetConsoleScreenBufferSize(hOut, bufferSize);
            cout << "\033[8;" << height << ";" << width << "t" << flush;
        } else {
            cout << "\033[8;" << height << ";" << width << "t" << flush;
            SetConsoleWindowInfo(hOut, TRUE, &windowSize);
            SetConsoleScreenBufferSize(hOut, bufferSize);
        }
    } else {
        if (enlarge) {
            SetConsoleScreenBufferSize(hOut, bufferSize);
            SetConsoleWindowInfo(hOut, TRUE, &windowSize);
        } else {
            SetConsoleWindowInfo(hOut, TRUE, &windowSize);
            SetConsoleScreenBufferSize(hOut, bufferSize);
        }
    }
    #elif defined(__APPLE__) || defined(__linux__)
    // ⭐️ Mac / Linux 端改用 cout 直出，避開 PrintBuilder 暫時物件帶來的風險
    cout << "\033[8;" << height << ";" << width << "t" << flush;
    #endif
}