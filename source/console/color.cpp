#include "console.h"
#include "data.h"
#include "input.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <string>
#include <iostream>

using std::string, std::to_string, std::cout, std::flush;

extern Data dta;

static void parseHex(const string& hexColor, int& r, int& g, int& b) {
    if (hexColor.length() < 6) return;
    try {
        r = std::stoi(hexColor.substr(0, 2), nullptr, 16);
        g = std::stoi(hexColor.substr(2, 2), nullptr, 16);
        b = std::stoi(hexColor.substr(4, 2), nullptr, 16);
    } catch (...) {
        r = 255; g = 255; b = 255;
    }
}

#ifdef _WIN32
static const int palette[16][3] = {
    {0,0,0},       {0,0,128},     {0,128,0},     {0,128,128},
    {128,0,0},     {128,0,128},   {128,128,0},   {192,192,192},
    {128,128,128}, {0,0,255},     {0,255,0},     {0,255,255},
    {255,0,0},     {255,0,255},   {255,255,0},   {255,255,255},
};

static int nearestColor(int r, int g, int b) {
    int best = 0;
    double bestDist = 1e18;
    for (int i = 0; i < 16; i++) {
        double dr = r - palette[i][0];
        double dg = g - palette[i][1];
        double db = b - palette[i][2];
        double dist = dr*dr + dg*dg + db*db;
        if (dist < bestDist) { bestDist = dist; best = i; }
    }
    return best;
}
#endif

// ⭐️ 套用前景 (Fg: 文字顏色)
void Console::applyFg(const string& hex) {
    if (hex.empty()) return;
    int r = 255, g = 255, b = 255;
    parseHex(hex, r, g, b);
    
    #ifdef _WIN32
    if (dta.cfg.vt100Color == 1) {
        cout << "\033[38;2;" << r << ";" << g << ";" << b << "m" << flush;
    } else {
        int colorIdx = nearestColor(r, g, b);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        WORD attr = (colorIdx & 0x0F) | (csbi.wAttributes & 0xF0);
        SetConsoleTextAttribute(hOut, attr);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    cout << "\033[38;2;" << r << ";" << g << ";" << b << "m" << flush;
    #endif
}

// ⭐️ 套用背景 (Bg: 底色)
void Console::applyBg(const string& hex) {
    if (hex.empty()) return;
    int r = 0, g = 0, b = 0;
    parseHex(hex, r, g, b);
    
    #ifdef _WIN32
    if (dta.cfg.vt100Color == 1) {
        cout << "\033[48;2;" << r << ";" << g << ";" << b << "m" << flush;
    } else {
        int colorIdx = nearestColor(r, g, b);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        WORD attr = (csbi.wAttributes & 0x0F) | ((colorIdx & 0x0F) << 4);
        SetConsoleTextAttribute(hOut, attr);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    cout << "\033[48;2;" << r << ";" << g << ";" << b << "m" << flush;
    #endif
}

Console::ColorSetter Console::setColor(const string& hexColor) {
    string backup = globalFgColor;
    globalFgColor = hexColor;
    applyFg(hexColor);
    return ColorSetter(this, false, hexColor, backup);
}

// 1. fillScreenBg：刷滿全螢幕背景
void Console::fillScreenBg(const string& hexColor) {
    globalBgColor = hexColor; 
    applyBg(hexColor);

    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD savedPos = csbi.dwCursorPosition;

    if (dta.cfg.vt100Color == 1) {
        cout << "\033[2J\033[3J" << flush;
        SetConsoleCursorPosition(hOut, savedPos);
    } else {
        COORD origin = {0, 0};
        DWORD totalCells = csbi.dwSize.X * csbi.dwSize.Y;
        DWORD written;

        FillConsoleOutputCharacter(hOut, ' ', totalCells, origin, &written);
        FillConsoleOutputAttribute(hOut, csbi.wAttributes, totalCells, origin, &written);
        
        SetConsoleTextAttribute(hOut, csbi.wAttributes);
        SetConsoleCursorPosition(hOut, savedPos);
    }
    #else
    cout << "\033[6n" << flush;
    isQuary = true;
    while(isQuary);
    cout << "\033[2J\033[3J" << flush;
    cout << "\033[" << cursorRow << ";" << cursorCol << "H" << flush;
    #endif
}

// 2. setColorBg
Console::ColorSetter Console::setColorBg(const string& hexColor) {
    string backup = globalBgColor;
    globalBgColor = hexColor;
    applyBg(hexColor);
    return ColorSetter(this, true, hexColor, backup);
}

// ⭐️ 新增：取消析構時的 fillScreenBg
Console::ColorSetter& Console::ColorSetter::noFill() {
    isFill = false;
    return *this;
}

// ⭐️ 新增：設定為一次性顏色，輸出一次後還原
Console::ColorSetter& Console::ColorSetter::once() {
    isOnce = true;
    if (isBg) {
        parent->onceBg = true;
        parent->onceBgBackup = backup;
    } else {
        parent->onceFg = true;
        parent->onceFgBackup = backup;
    }
    return *this;
}

void Console::resetColor() {
    if (onceFg) {
        globalFgColor = onceFgBackup;
        applyFg(globalFgColor);
        onceFg = false;
    }
    if (onceBg) {
        globalBgColor = onceBgBackup;
        applyBg(globalBgColor);
        onceBg = false;
    }
}

Console::ColorSetter::~ColorSetter() {
    // 只有在是背景色 setting 且 isFill 為 true 時才刷滿螢幕
    if (isBg && isFill) {
        parent->fillScreenBg(hex);
    }
}