#define _HAS_STD_BYTE 0
#include "console.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <iostream>
#include <thread>
#endif
using namespace std;

void Console::printAt(const int x, const int y, const string& text) {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD origPos = csbi.dwCursorPosition;
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
    DWORD written;
    WriteConsoleA(hOut, text.c_str(), (DWORD)text.size(), &written, NULL);
    SetConsoleCursorPosition(hOut, origPos);
    #elif __APPLE__
    cout << "\033[s";
    cout << "\033[" << (x+1) << ";" << (y+1) << "H" << text;
    cout << "\033[u";
    cout.flush();
    #endif
}