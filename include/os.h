#pragma once
#include "HNCIP.h"
#include <tuple>
#include <vector>
#include <string>
#include <functional>

class hnfcOS {
public:
    hnfcOS() : app(this) {}
    void Boot();
    void Initial(bool full);
    void Interface();
private:
    void MenuBar();
    std::string Mode;
    std::string targetIP;
    std::string path;
    HNCInterPreter::NodeInfo node;
    std::vector<std::function<void()>> termTasks;
    void Display();
    int displayChse;
    void Terminal();
    struct Application {
    public:
        Application(hnfcOS* p) : parent(p) {} 
        void Probe(HNCInterPreter::NodeInfo& node);
        bool Probed;
        void FileView(HNCInterPreter::NodeInfo& node);
    private:
        hnfcOS* parent;
    };
    Application app;
    HNCInterPreter::NodeInfo getNode(const std::string &targetIP);
};