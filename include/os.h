#pragma once
#include "HNCIP.h"
#include <tuple>
#include <vector>
#include <string>
#include <functional>

class hnfcOS {
public:
    hnfcOS() : cmd(this), app(this), kit(this) {}
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
        void Concatenate(const std::string &targetName, HNCInterPreter::NodeInfo& node);
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
    private:
        hnfcOS* parent;
    };
    Application app;
    struct Kit {
    public:
        Kit(hnfcOS* p) : parent(p) {} 
        void regFolder(HNCInterPreter::NodeInfo::FolderEntry &f, std::string &path, int &i, int &indent, std::vector<std::string> &objectNames, std::vector<HNCInterPreter::NodeInfo::FolderEntry>* siblings = nullptr);
        void regFile(HNCInterPreter::NodeInfo::FileEntry &f, std::string &path, int &i, int &indent, std::vector<std::string> &objectNames);
        void viewFile(const std::string &name, const std::vector<std::string> &contents, bool &opened);
    private:
        hnfcOS* parent;
    };
    Kit kit;
    HNCInterPreter::NodeInfo getNode(const std::string &targetIP);
};