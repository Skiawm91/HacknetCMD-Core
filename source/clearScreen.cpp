#define _HAS_STD_BYTE 0
#include "clearScreen.h"
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
void cls() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD topLeft = {0, 0};
    DWORD count;
    FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, topLeft, &count);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, topLeft, &count);
    SetConsoleCursorPosition(hOut, topLeft);
}
#endif