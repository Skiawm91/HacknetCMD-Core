#define _HAS_STD_BYTE 0
#include "console.h"
#include "data.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include "input.h"
#include <regex>
#endif
#include <iostream>
using namespace std;

extern Data dta;

#ifdef _WIN32
void Console::printAt(const int x, const int y, const string& text) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD origPos = csbi.dwCursorPosition;
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
    DWORD written;
FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X - x, pos, &written);
FillConsoleOutputAttribute(hOut, csbi.wAttributes, csbi.dwSize.X - x, pos, &written);
    WriteConsoleA(hOut, text.c_str(), (DWORD)text.size(), &written, NULL);
    // 自動重置
    if (dta.cfg.vt100color == 1) {
        cout << "\033[0m";
        cout.flush();
    } else {
        SetConsoleTextAttribute(hOut, 7);
    }
    SetConsoleCursorPosition(hOut, origPos);
}
#elif defined(__APPLE__) || defined(__linux__)
void Console::printAt(int x, int y, const std::string& text) {
    std::cout << "\033[6n" << flush;
    isQuary = true;
    while(isQuary);
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H";
    std::cout << "\033[K";
    std::cout << text;
    std::cout << "\033[0m";
    std::cout << "\033[" << cursorRow << ";" << cursorCol << "H";
    std::cout.flush();
}
#endif

void Console::PrintAtExtension::noBack(int x, int y, const std::string& text) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD origPos = csbi.dwCursorPosition;
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
    DWORD written;
FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X - x, pos, &written);
FillConsoleOutputAttribute(hOut, csbi.wAttributes, csbi.dwSize.X - x, pos, &written);
    WriteConsoleA(hOut, text.c_str(), (DWORD)text.size(), &written, NULL);
    // 自動重置
    if (dta.cfg.vt100color == 1) {
        cout << "\033[0m";
        cout.flush();
    } else {
        SetConsoleTextAttribute(hOut, 7);
    }
#elif defined(__APPLE__) || defined(__linux__)
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H";
    std::cout << "\033[K";
    std::cout << text;
    std::cout << "\033[0m";
    std::cout.flush();
#endif
}