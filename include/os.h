#pragma once
#include "HNCIP.h"
#include <tuple>
#include <vector>
#include <string>
#include <functional>

class hnfcOS {
public:
    hnfcOS() : cmd(this), app(this) {}
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
    struct Command {
    public:
        Command(hnfcOS* p) : parent(p) {} 
        void ChangeDir(HNCInterPreter::NodeInfo& node, const std::string &dir = "");
        void ListDir(HNCInterPreter::NodeInfo& node);
    private:
        hnfcOS* parent;
    };
    Command cmd;
    struct Application {
    public:
        Application(hnfcOS* p) : parent(p) {} 
        void Probe(HNCInterPreter::NodeInfo& node);
        bool Probed;
        void FileView(HNCInterPreter::NodeInfo& node);
        void regFolder(HNCInterPreter::NodeInfo::FolderEntry &f, string &path, int &i, int &indent, vector<string> &objectNames, vector<HNCInterPreter::NodeInfo::FolderEntry>* siblings = nullptr);
    private:
        hnfcOS* parent;
    };
    Application app;
    HNCInterPreter::NodeInfo getNode(const std::string &targetIP);
};