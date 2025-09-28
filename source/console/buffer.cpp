#define _HAS_STD_BYTE 0
#include "console.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <vector>
using namespace std;

#ifdef _WIN32
void Console::bufferSave(int startRow) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;
    int totalRows = csbi.dwSize.Y;
    int width = csbi.dwSize.X;
    vector<CHAR_INFO> screenBuffer(totalRows * width);
    COORD bufferSize = { (SHORT)width, (SHORT)totalRows };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT readRegion = { 0, (SHORT)startRow, (SHORT)(width - 1), (SHORT)(totalRows - 1) };
    ReadConsoleOutputA(hOut, screenBuffer.data(), bufferSize, bufferCoord, &readRegion);
    int lastContentRow = -1;
    for (int row = 0; row < totalRows - startRow; ++row) {
        bool hasContent = false;
        for (int col = 0; col < width; ++col) {
            if (screenBuffer[row * width + col].Char.AsciiChar != ' ') {
                hasContent = true;
                break;
            }
        }
        if (hasContent) lastContentRow = row;
    }
    if (lastContentRow < 0) {
        savedBuffer.clear();
        savedWidth = savedHeight = savedRow = 0;
        return;
    }
    savedWidth = width;
    savedHeight = lastContentRow + 1;
    savedRow = startRow;
    savedBuffer.resize(savedWidth * savedHeight);
    for (int row = 0; row < savedHeight; ++row) {
        for (int col = 0; col < savedWidth; ++col) {
            savedBuffer[row * savedWidth + col] = screenBuffer[row * width + col];
        }
    }
}
void Console::bufferRestore() {
    if (savedBuffer.empty()) return;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;
    COORD bufferSize = { (SHORT)savedWidth, (SHORT)savedHeight };
    COORD bufferCoord = { 0, 0 };
    SMALL_RECT writeRegion = { 0, (SHORT)savedRow, (SHORT)(savedWidth - 1), (SHORT)(savedRow + savedHeight - 1) };
    WriteConsoleOutputA(hOut, savedBuffer.data(), bufferSize, bufferCoord, &writeRegion);
    SHORT newCursorY = (SHORT)(savedRow + savedHeight);
    if (newCursorY >= csbi.dwSize.Y) {
        SHORT scrollAmount = newCursorY - (csbi.dwSize.Y - 1);
        SMALL_RECT scrollRect = { 0, 0, csbi.dwSize.X - 1, csbi.dwSize.Y - 1 };
        COORD destOrigin = { 0, (SHORT)(-scrollAmount) };
        CHAR_INFO fill{};
        fill.Char.AsciiChar = ' ';
        fill.Attributes = csbi.wAttributes;
        ScrollConsoleScreenBufferA(hOut, &scrollRect, nullptr, destOrigin, &fill);
        newCursorY = (SHORT)(csbi.dwSize.Y - 1);
    }
    COORD newCursor = { 0, newCursorY };
    SetConsoleCursorPosition(hOut, newCursor);
}
#endif

#ifdef __APPLE__
void Console::bufferChange(int mode) {
    if (mode == 0) cout << "\033[?1049l" << flush;
    else if (mode == 1) cout << "\033[?1049h" << flush;
}
#endif