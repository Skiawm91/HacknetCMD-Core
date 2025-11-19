#define _HAS_STD_BYTE 0
#include "os.h"
#include "console.h"
#include "HNCIP.h"
#include "input.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

extern Console con;
extern HNCInterPreter hncip;
extern ManageInput mi;
extern string playerLang;

void hnfcOS::Kit::viewFile(const string &name, const vector<string> &contents, bool &opened, vector<string> &objectNames) {
    mi.mouse.btnDel(objectNames);
    #ifdef _WIN32
    con.clear();
    #elif __APPLE
    con.clearBuf2();
    #endif
    parent->MenuBar();
    objectNames.push_back("BACK2");
    std::cout << " " << name << " | Back |\n" << std::endl;
    mi.mouse.btnAdd("BACK2", 2 + name.size(), 3, 8, 1);
    mi.mouse.cbCreate("BACK2", [&opened](const string &btnName){
        if (btnName == "BACK2") opened = false;
    });
    for (const auto &c : contents) {
        std::cout << c << std::endl;
    }
    mi.async(3);
}

void hnfcOS::Kit::regFile(HNCInterPreter::NodeInfo::FileEntry &f, string &path, int &i, int &indent, vector<string> &objectNames) {
    string objName = f.name + to_string(i);
    objectNames.push_back(objName);
    hncip.script("hnfcOS/display/fileview.chns", "FILE", vector<string>{"FILENAME", "BLOCK"}, vector<string>{f.name, string(indent, ' ')});
    mi.mouse.btnAdd(objName, 1, 4 + i, 30, 3);
    auto *ptr = &f;
    mi.mouse.cbCreate(objName, [ptr, objName, &objectNames, this](const string& btnName){
        if (btnName == objName || ptr->opened) {
            ptr->opened = true;
            parent->kit.viewFile(ptr->name, ptr->contents, ptr->opened, objectNames);
        }
    });
    i += 2;
}

void collapseAll(HNCInterPreter::NodeInfo::FolderEntry* f, vector<function<void()>>& tasks, string& path, const string& targetIP) {
    if (!f) return;

    // 遞迴收 subfolders
    for (auto &sf : f->subfolders) {
        if (sf.expand != 0) {
            collapseAll(&sf, tasks, path, targetIP); // deeper first
            sf.expand = 0;
        }
    }

    // 自己也要收回 (cd ..)
    tasks.push_back([targetIP, path]() {
        if (!targetIP.empty()) {
            if (!path.empty()) std::cout << targetIP << path << "> cd .." << std::endl;
            else std::cout << targetIP << "@> cd .." << std::endl;
        } else cout << "> cd .." << std::endl;
    });

    // 修正 path（只 pop 一次）
    if (!path.empty()) {
        if (path.back() == '/') path.pop_back();
        size_t pos = path.rfind('/');
        if (pos != string::npos) path.erase(pos + 1);
    }
}


void hnfcOS::Kit::regFolder(HNCInterPreter::NodeInfo::FolderEntry &f, string &path, int &i, int &indent, vector<string> &objectNames, vector<HNCInterPreter::NodeInfo::FolderEntry>* siblings) {
    string objName = f.name + to_string(i);
    objectNames.push_back(objName);
    hncip.script("hnfcOS/display/fileview.chns", "FOLDER_" + to_string(f.expand), vector<string>{"FOLDERNAME", "BLOCK"}, vector<string>{f.name, string(indent, ' ')});
    mi.mouse.btnAdd(objName, 1, 4 + i, 30, 3);

    auto *ptr = &f;
    mi.mouse.cbCreate(objName, [ptr, objName, &path, siblings, this](const string& btnName){
        if (btnName == objName) {
            if (ptr->expand == 0) ptr->expand = 1;
            else ptr->expand = 0;
            if (ptr->expand == 1) {
                if (siblings) {
                    for (auto &sibling : *siblings) {
                        if (&sibling != ptr && sibling.expand != 0) {
                            collapseAll(&sibling, parent->termTasks, path, parent->targetIP);
                            sibling.expand = 0;
                        }
                    }
                }
                parent->termTasks.push_back([targetIP = parent->targetIP, path = path, ptr](){
                    if (!targetIP.empty()) {
                        if (!path.empty()) std::cout << targetIP << path << "> cd " << ptr->name << std::endl;
                        else std::cout << targetIP << "@> cd " << ptr->name << std::endl;
                    } else cout << "> cd " << ptr->name << std::endl;
                });
                path = (path.empty() ? "/" : path) + ptr->name + "/";
                ptr->expand = 2;
            } else if (ptr->expand == 0) {
                for (auto &sf : ptr->subfolders) {
                    if (sf.expand != 0) {
                        collapseAll(&sf, parent->termTasks, path, parent->targetIP);
                        sf.expand = 0;
                    }
                }
                parent->termTasks.push_back([targetIP = parent->targetIP, path = path, ptr](){
                    if (!targetIP.empty()) {
                        if (!path.empty()) std::cout << targetIP << path << "> cd .." << std::endl;
                        else std::cout << targetIP << "@> cd .." << std::endl;
                    } else cout << "> cd .." << std::endl;
                });
                if (path.back() == '/') path.pop_back();
                size_t pos = path.rfind('/');
                if (pos != string::npos) path.erase(pos + 1);
            }
        }
    });
    i += 2;
    if (f.expand == 2) {
        int childIndent = indent + 1;
        for (auto &sf : f.subfolders) {
            regFolder(sf, path, i, childIndent, objectNames, &f.subfolders);
        }
        for (auto &fi : f.files) {
            regFile(fi, path, i, childIndent, objectNames);
        }
    }
}

void hnfcOS::Application::FileView(HNCInterPreter::NodeInfo& node) {
    bool back = false;
    while (!back) {
        #ifdef _WIN32
        con.clear();
        #elif __APPLE__
        con.clearBuf2();
        #endif
        parent->MenuBar();
        int i = 0, indent = 0;
        vector<string> objectNames;
        hncip.script("hnfcOS/display/fileview.chns", "TOPLINE_" + playerLang, vector<string>{"NODENAME"}, vector<string>{node.Name});
        // fs start
        for (auto &f : node.folders) {
            parent->kit.regFolder(f, parent->path, i, indent, objectNames, &node.folders);
        }
        for (auto &f : node.files) {
            parent->kit.regFile(f, parent->path, i, indent, objectNames);
        }
        objectNames.push_back("BACK");
        hncip.script("hnfcOS/display/fileview.chns", "BACK");
        mi.mouse.btnAdd("BACK", 1, 4 + i, 30, 3);
        mi.mouse.cbCreate("BACK", [&](const string& btnName){
            if (btnName == "BACK" || btnName == "EXIT" ||
                btnName == "SETTINGS" || btnName == "SAVE" ||
                btnName == "TERMINAL" || btnName == "DISPLAY" ||
                btnName == "NETMAP" || btnName == "RAM" || btnName == "MAIL") back = true;
            if (btnName == "BACK") parent->displayChse = 0;
        });
        mi.async(3);
        mi.mouse.btnDel(objectNames);
        for (const auto &oN : objectNames) {
            mi.mouse.cbClean(oN);
        }
    }
}