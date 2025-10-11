#pragma once
#include "HNCIP.h"
#include <tuple>
#include <vector>
#include <string>

class hnfcOS {
public:
    hnfcOS() : app(this) {}
    void Boot();
    void Initial(bool full);
    void Interface();
private:
    void MenuBar();
    std::string targetIP;
    void Display();
    void Terminal();
    struct Application {
    public:
        Application(hnfcOS* p) : parent(p) {} 
        void Probe(const HNCInterPreter::NodeInfo& node);
    private:
        hnfcOS* parent;
    };
    Application app;
    HNCInterPreter::NodeInfo getNode();
};