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
    COORD bufferSize;
    bufferSize.X = x;
    bufferSize.Y = y;
    SetConsoleScreenBufferSize(hOut, bufferSize);
    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = bufferSize.X - 1;
    windowSize.Bottom = bufferSize.Y - 1;
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    #elif __APPLE__
    cout << "\033[8;" << y << ";" << x << "t";
    cout.flush();
    #endif
}