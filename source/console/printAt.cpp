#define _HAS_STD_BYTE 0
#include "console.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <iostream>
#include <termios.h>
#include <unistd.h>
#endif
using namespace std;

#ifdef __APPLE__
std::pair<int,int> getCursorPosition() {
    std::cout << "\033[6n" << std::flush; // DSR
    char buf[32];
    int i = 0;
    char ch;
    while (read(STDIN_FILENO, &ch, 1) == 1) {
        buf[i++] = ch;
        if (ch == 'R') break;
    }
    buf[i] = '\0';
    int row = 0, col = 0;
    if (sscanf(buf, "\033[%d;%dR", &row, &col) == 2)
        return {row, col};
    return {0, 0};
}
#endif

#ifdef _WIN32
void Console::printAt(const int x, const int y, const string& text) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD origPos = csbi.dwCursorPosition;
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
    DWORD written;
    WriteConsoleA(hOut, text.c_str(), (DWORD)text.size(), &written, NULL);
    SetConsoleCursorPosition(hOut, origPos);
}
#elif __APPLE__
void Console::printAt(const int x, const int y, const int backX, const int backY, const string& text) {
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H" << text;
    std::cout << "\033[" << (backY+1) << ";" << (backX+1) << "H";
    std::cout.flush();
}
#endif