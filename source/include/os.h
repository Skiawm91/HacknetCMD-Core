#pragma once
#include <string>
class hnfcOS {
public:
    void Boot();
    void Initial(bool full);
    void Interface();
private:
    std::string targetIP;
    void Display();
    void Terminal();
};