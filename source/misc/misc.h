#pragma once
#include <string>
class Misc {
public:
    std::string toLangName(const int code);
    void staticPrint(const int x, const int y, const std::string& text);
};