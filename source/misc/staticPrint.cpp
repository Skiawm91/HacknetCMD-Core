#define _HAS_STD_BYTE 0
#include "misc.h"
#include <string>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

void Misc::staticPrint(const int x, const int y, const string& text) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
    cout << text;
    cout.flush();
}