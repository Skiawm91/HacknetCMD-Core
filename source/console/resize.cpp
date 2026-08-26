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
    if (dta.cfg.vt100Resize == 1) {
        print("\033[8;" + to_string(height) + ";" + to_string(width) + "t");
    } else {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        int curX = csbi.dwSize.X;
        int curY = csbi.dwSize.Y;
        bool enlarge = (width > curX) || (height > curY);
        COORD bufferSize = {(SHORT)width, (SHORT)height};
        SMALL_RECT windowSize = {0, 0, (SHORT)(width - 1), (SHORT)(height - 1)};
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