#pragma once
#include <tuple>
#include <vector>
#include <string>
class hnfcOS {
public:
    hnfcOS() : network(this) {}
    void Boot();
    void Initial(bool full);
    void Interface();
    struct Network {
    public:
        Network(hnfcOS* p) : parent(p) {}
        std::tuple<std::string, std::string, int, std::vector<std::string>, std::vector<int>, bool> List();
    private:
        hnfcOS* parent;
    };
    Network network;
private:
    std::string targetIP;
    void Display();
    void Terminal();
};