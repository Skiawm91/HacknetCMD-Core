#include "console.h"
#include "data.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <string>
#include <iostream>
using std::string, std::cout;

extern Data dta;

static void parseHex(const string& hexColor, int& r, int& g, int& b) {
    r = stoi(hexColor.substr(0, 2), nullptr, 16);
    g = stoi(hexColor.substr(2, 2), nullptr, 16);
    b = stoi(hexColor.substr(4, 2), nullptr, 16);
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

void Console::colorbg(const string& hexColor) {
    int r, g, b;
    parseHex(hexColor, r, g, b);
#ifdef _WIN32
    if (dta.cfg.vt100color == 1) {
        cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
        cout.flush();
    } else {
        int colorIdx = nearestColor(r, g, b);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        WORD attr = (csbi.wAttributes & 0x0F) | (colorIdx << 4);
        SetConsoleTextAttribute(hOut, attr);
    }
#elif defined(__APPLE__) || defined(__linux__)
    cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
    cout.flush();
#endif
}

void Console::color(const string& hexColor) {
    int r, g, b;
    parseHex(hexColor, r, g, b);
#ifdef _WIN32
    if (dta.cfg.vt100color == 1) {
        cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
        cout.flush();
    } else {
        int colorIdx = nearestColor(r, g, b);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        WORD attr = colorIdx | (csbi.wAttributes & 0xF0);
        SetConsoleTextAttribute(hOut, attr);
    }
#elif defined(__APPLE__) || defined(__linux__)
    cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
    cout.flush();
#endif
}