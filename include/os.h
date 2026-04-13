#pragma once
#include "HNCIP.h"
#include <tuple>
#include <vector>
#include <string>
#include <functional>

class hnfcOS {
public:
    hnfcOS() : sys(this), cmd(this), app(this), kit(this) {}
    void Boot();
    void Initial(bool full);
    void Interface();
private:
    void MenuBar();
    std::string Mode;
    std::string targetIP;
    std::string path;
    HNCInterPreter::NodeInfo::FileEntry* file = nullptr;
    HNCInterPreter::NodeInfo node;
    void Terminal();
    std::vector<std::function<void()>> termTasks;
    void Display();
    int displayChse;
    void NetMap();
    void RAMUI();
    void MailUI();
    struct System {
    public:
        System(hnfcOS* p) : parent(p) {}
        HNCInterPreter::NodeInfo getNode(const std::string &targetIP);
        void cleanNode() {
            parent->targetIP.clear();
            parent->path.clear();
            parent->file = nullptr;
            parent->node = getNode(parent->targetIP);
            parent->displayChse = 0;
        }
    private:
        hnfcOS* parent;
    };
    System sys;
    struct Command {
    public:
        Command(hnfcOS* p) : parent(p) {} 
        void ChangeDir(HNCInterPreter::NodeInfo& node, const std::string &dir = "");
        void ListDir(HNCInterPreter::NodeInfo& node);
        void Concatenate(const std::string &targetName, HNCInterPreter::NodeInfo& node);
        void Remove(const string& path, HNCInterPreter::NodeInfo& node);
    private:
        hnfcOS* parent;
    };
    Command cmd;
    struct Application {
    public:
        Application(hnfcOS* p) : parent(p) {} 
        void Probe(HNCInterPreter::NodeInfo& node);
        void FileView(HNCInterPreter::NodeInfo& node);
    private:
        hnfcOS* parent;
        bool Probed;
    };
    Application app;
    struct Kit {
    public:
        Kit(hnfcOS* p) : parent(p) {} 
        void regFolder(HNCInterPreter::NodeInfo::FolderEntry &f, std::string thisPath, std::string &path, int &i, int &indent, std::vector<std::string> &objectNames, std::vector<HNCInterPreter::NodeInfo::FolderEntry>* siblings = nullptr);
        void regFile(HNCInterPreter::NodeInfo::FileEntry &f, std::string &path, int &i, int &indent, std::vector<std::string> &objectNames);
        void viewFile(const std::string &name, const std::vector<std::string> &contents);
        void saveNode(const HNCInterPreter::NodeInfo& node);
    private:
        hnfcOS* parent;
    };
    Kit kit;
};