#pragma once
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <optional>
#endif
#include <string>
#include <vector>
class Console {
public:
    void clear();
    void resize(const int width, const int height);
    #ifdef _WIN32
    void printAt(int x, int y, const std::string& text);
    void bufferSave(int startRow = 0);
    void bufferRestore();
    #elif __APPLE__
    void printAt(const int x, const int y, const int backX, const int backY, const std::string& text);
    void bufferChange(int mode = 0);
    #endif
private:
    #ifdef _WIN32
    std::vector<CHAR_INFO> savedBuffer;
    int savedWidth = 0;
    int savedHeight = 0;
    int savedRow = 0;
    #elif __APPLE__
    std::vector<std::string> savedLines;
    #endif
};