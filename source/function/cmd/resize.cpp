#define _HAS_STD_BYTE 0
#include "../function.h"
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <iostream>
#endif
using namespace std;

void Function::CMD::resize(const int x, const int y) {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    int curX = csbi.dwSize.X;
    int curY = csbi.dwSize.Y;
    bool enlarge = (x > curX) || (y > curY);
    COORD bufferSize = {(SHORT)x, (SHORT)y};
    SMALL_RECT windowSize = {0, 0, (SHORT)(x - 1), (SHORT)(y - 1)};
    if (enlarge) {
        SetConsoleScreenBufferSize(hOut, bufferSize);
        SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    } else {
        SetConsoleWindowInfo(hOut, TRUE, &windowSize);
        SetConsoleScreenBufferSize(hOut, bufferSize);
    }
    #elif __APPLE__
    cout << "\033[8;" << y << ";" << x << "t";
    cout.flush();
    #endif
}