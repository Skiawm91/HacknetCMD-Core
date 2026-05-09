#define _HAS_STD_BYTE 0
#include "console.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include "input.h"
#include <iostream>
#include <regex>
#endif
using namespace std;

#ifdef _WIN32
void Console::printAt(const int x, const int y, const string& text) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD origPos = csbi.dwCursorPosition;
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, pos);
    DWORD written;
    FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X, pos, &written);
    WriteConsoleA(hOut, text.c_str(), (DWORD)text.size(), &written, NULL);
    SetConsoleCursorPosition(hOut, origPos);
}
#elif defined(__APPLE__) || defined(__linux__)
void Console::printAt(int x, int y, const std::string& text) {
    std::cout << "\033[6n" << flush; // 透過 Input 函式來幫助取得
    isQuary = true;
    while(isQuary); // 換成等待完成
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H";
    std::cout << "\033[2K";
    std::cout << text;
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
    FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X, pos, &written);
    WriteConsoleA(hOut, text.c_str(), (DWORD)text.size(), &written, NULL);
    #elif defined(__APPLE__) || defined(__linux__)
    std::cout << "\033[" << (y+1) << ";" << (x+1) << "H";
    std::cout << "\033[2K";
    std::cout << text;
    std::cout.flush();
    #endif
}