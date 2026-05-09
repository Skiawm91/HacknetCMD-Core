#define _HAS_STD_BYTE 0
#include "console.h"
#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <iostream>
using std::cout;
#endif

void Console::Cursor::show() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
    #elif defined(__APPLE__) || defined(__linux__)
    cout << "\033[?25h";
    cout.flush();
    #endif
}

void Console::Cursor::hide() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
    #elif defined(__APPLE__) || defined(__linux__)
    cout << "\033[?25l";
    cout.flush();
    #endif
}