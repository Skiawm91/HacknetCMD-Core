#define _HAS_STD_BYTE 0
#include "ASMScript.h"
#include "../../crypto/crypto.h"
#include "../../input/input.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

string input;
extern ManageInput mi;

#ifdef _WIN32
void OTTF2K() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    if (csbi.dwCursorPosition.Y > 0) {
        COORD pos = { 0, csbi.dwCursorPosition.Y - 1 };
        DWORD written;
        FillConsoleOutputCharacter(hConsole, ' ', csbi.dwSize.X, pos, &written);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X, pos, &written);
        SetConsoleCursorPosition(hConsole, pos);
    }
}
#endif

void HNScript::GETINPUT(const string& content) {
    input.clear();
    cout << content;
    inputMasked = false;
    while(!enterDetected);
    input = mi.getInput();
}

void HNScript::GETINPUTR(const string& content) {
    input.clear();
    #ifdef _WIN32
    OTTF2K();
    #else
    cout << "\033[F\033[2K";
    #endif
    cout << content;
    inputMasked = false;
    while(!enterDetected);
    input = mi.getInput();
}

void HNScript::GETINPUTPWD(const string& content) {
    input.clear();
    cout << content;
    inputMasked = true;
    while(!enterDetected);
    input = mi.getInput();
}

void HNScript::GETINPUTPWDR(const string& content) {
    input.clear();
    #ifdef _WIN32
    OTTF2K();
    #else
    cout << "\033[F\033[2K";
    #endif
    cout << content;
    inputMasked = true;
    while(!enterDetected);
    input = mi.getInput();
}