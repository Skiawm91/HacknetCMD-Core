#define _HAS_STD_BYTE 0
#include "clearScreen.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <iostream>
#endif

void cls() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD topLeft = {0, 0};
    DWORD count;
    FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, topLeft, &count);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, topLeft, &count);
    SetConsoleCursorPosition(hOut, topLeft);
    #else
    std::cout << "\033[2J\033[H" << std::flush;
    #endif
}