#include "console.h"
#include "data.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <string>
#include <iostream>

using std::string, std::cout, std::flush;

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

// 1. fillScreenBg：除了刷滿，還要將這個顏色設為 Windows 控制台的預設屬性
void Console::fillScreenBg(const string& hexColor) {
    globalBgColor = hexColor; // ⭐️ 更新全域背景色記錄
    applyBg(hexColor);

    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    COORD savedPos = csbi.dwCursorPosition;

    if (dta.cfg.vt100Color == 1) {
        // VT100: 清屏並刷滿當前背景色
        cout << "\033[2J" << flush;
        SetConsoleCursorPosition(hOut, savedPos);
    } else {
        // Win32 API: 刷滿屬性
        COORD origin = {0, 0};
        DWORD written;
        DWORD totalCells = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputAttribute(hOut, csbi.wAttributes, totalCells, origin, &written);
        
        // ⭐️ 關鍵：將預設控制台屬性設為當前背景，讓後續換行 (\n) 也自帶背景色
        SetConsoleTextAttribute(hOut, csbi.wAttributes);
        SetConsoleCursorPosition(hOut, savedPos);
    }
    #else
    cout << "\033[s\033[2J\033[u" << flush;
    #endif
}

// 2. setColorBg：回傳 ColorSetter，預設 isSingleLine 為 false
Console::ColorSetter Console::setColorBg(const string& hexColor) {
    string backup = globalBgColor;
    globalBgColor = hexColor;
    applyBg(hexColor);
    return ColorSetter(this, true, hexColor, backup);
}

// 1. singleLine(): 只是設定「當前這行要用 hex，但只用一次，下一次 resetColor 時恢復為 backup」
void Console::ColorSetter::singleLine() {
    isSingleLine = true; // 標記為單行，阻止 ~ColorSetter() 觸發 fillScreenBg 全螢幕刷色
    
    if (isBg) {
        // ⭐️ 保持當前的 globalBgColor 為 hex (橘色)
        // 記錄下 backup，讓後續 printAt 印完呼叫 resetColor() 時才還原！
        parent->singleLineBg = true;
        parent->singleLineBgBackup = backup;
    } else {
        parent->singleLineFg = true;
        parent->singleLineFgBackup = backup;
    }
}

// 2. resetColor(): 當 printAt 印完解構時，由 resetColor 把單行顏色還原回去！
void Console::resetColor() {
    if (singleLineFg) {
        globalFgColor = singleLineFgBackup;
        applyFg(globalFgColor);
        singleLineFg = false;
    }
    if (singleLineBg) {
        // ⭐️ 這裡才是真正還原全域背景色的地方！
        globalBgColor = singleLineBgBackup;
        applyBg(globalBgColor);
        singleLineBg = false;
    }
}

// 4. ColorSetter 的解構子 (Destructor)：決定要不要刷滿全螢幕
Console::ColorSetter::~ColorSetter() {
    if (isBg && !isSingleLine) {
        parent->fillScreenBg(hex);
    }
}