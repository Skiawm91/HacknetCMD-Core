#define _HAS_STD_BYTE 0
#include "console.h"
#include "data.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include "input.h"
#endif

using namespace std;
extern Data dta;

// --- PrintBuilder 實作 ---
Console::PrintBuilder Console::print(const string& text) {
    return PrintBuilder(this, text);
}

Console::PrintBuilder Console::println(const string& text) {
    return PrintBuilder(this, text + "\n");
}

Console::PrintBuilder& Console::PrintBuilder::save() {
    shouldSave = true;
    return *this;
}

Console::PrintBuilder::~PrintBuilder() {
    parent->applyFg(parent->getFg());
    parent->applyBg(parent->getBg());
    cout << text;
    cout.flush();

    if (shouldSave) {
        parent->addRecord({ -1, -1, text, parent->getFg(), parent->getBg(), false, true });
    }
}

// --- PrintAtBuilder 實作 ---
Console::PrintAtBuilder Console::printAt(int x, int y, const string& text) {
    return PrintAtBuilder(this, x, y, text);
}

Console::PrintAtBuilder& Console::PrintAtBuilder::noBack() {
    isNoBack = true;
    return *this;
}

Console::PrintAtBuilder& Console::PrintAtBuilder::save() {
    shouldSave = true;
    return *this;
}

Console::PrintAtBuilder& Console::PrintAtBuilder::noEraseEOL() {
    isNoEraseEOL = true;
    return *this;
}

Console::PrintAtBuilder::~PrintAtBuilder() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD originalPos = csbi.dwCursorPosition; // 備份原相對游標

    // 1. 移動到指定的絕對 (x, y) 位置繪製
    COORD targetPos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, targetPos);
    #else
    // POSIX VT100: 使用 ANSI 保存/移動游標
    if (!isNoBack) {
        std::cout << "\033[6n" << flush;
        isQuary = true;
        while(isQuary);
    }
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
    #endif

    // 2. 套用顏色與繪製文字
    parent->applyFg(parent->getFg());
    parent->applyBg(parent->getBg());
    #ifdef _WIN32
    if (dta.cfg.vt100Color == 1) {
        if (!isNoEraseEOL) cout << "\033[K";
        cout << text;
        cout.flush();
    } else {
        DWORD written;
        if (!isNoEraseEOL) FillConsoleOutputCharacter(hOut, ' ', csbi.dwSize.X - x, targetPos, &written);
        WriteConsoleA(hOut, text.c_str(), (DWORD)text.size(), &written, NULL);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    if (!isNoEraseEOL) cout << "\033[K";
    cout << text;
    cout.flush();
    #endif

    // 3. ⭐️ 關鍵：判斷是否需要返回 (Back) 舊游標
    if (!isNoBack) {
        // 普通 PRINTAT：印完必須跳回原本的相對游標點！
        #ifdef _WIN32
        SetConsoleCursorPosition(hOut, originalPos);
        #else
        // ⭐️ 改用 cout 直出，絕對不在解構子裡呼叫 print()
        cout << "\033[" << cursorRow << ";" << cursorCol << "H" << flush;
        #endif
    } else {
        // ⭐️ PRINTAT_NB (No Back)：不返回！
        // 游標直接停在剛印完的位置末尾，讓腳本後面的 PRINT 來推動換行！
    }

    parent->resetColor();
}