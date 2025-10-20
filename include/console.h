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
    void printAt(int x, int y, const std::string& text);
    struct PrintAtExtension {
        void noBack(int x, int y, const std::string& text);
    };
    PrintAtExtension pae;
    #ifdef _WIN32
    void bufferSave(int startRow = 0);
    void bufferRestore();
    #elif __APPLE__
    void clearBuf2();
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