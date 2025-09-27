#pragma once
#ifdef _WIN32
#include <windows.h>
#endif
#include <string>
#include <vector>
class Console {
public:
    void clear();
    void resize(const int width, const int height);
    void printAt(int x, int y, const std::string& text);
    void bufferSave(int startRow = 0);
    void bufferRestore();
private:
    std::vector<CHAR_INFO> savedBuffer;
    int savedWidth = 0;
    int savedHeight = 0;
    int savedRow = 0;
};