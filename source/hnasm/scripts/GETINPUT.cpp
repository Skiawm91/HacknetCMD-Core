#define _HAS_STD_BYTE 0
#include "CHNScript.h"
#include "../../crypto/crypto.h"
#include "../../input/input.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

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

void CHNScript::GETINPUT(const string& content) {
    kbPrompt = content;
    inputMasked = false;
    inputAte = false;
}

void CHNScript::GETINPUTR(const string& content) {
    #ifdef _WIN32
    OTTF2K();
    #else
    cout << "\033[F\033[2K";
    #endif
    kbPrompt = content;
    inputMasked = false;
    inputAte = false;
}

void CHNScript::GETINPUTPWD(const string& content) {
    kbPrompt = content;
    inputMasked = true;
    inputAte = false;
}

void CHNScript::GETINPUTPWDR(const string& content) {
    kbPrompt = content;
    inputMasked = true;
    inputAte = false;
}

void CHNScript::GETINPUTNUL(const string& content) {
    kbPrompt = content;
    inputMasked = false; // In fact, idk:)
    inputAte = true;
}