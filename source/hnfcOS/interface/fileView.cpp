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

void hnfcOS::Kit::viewFile(const string &name, const vector<string> &contents) {
    con.clear();
    parent->MenuBar();
    con.println("\n\n " + name + " | Back |");
    mi.mouse.btnAdd("BACK", 2 + name.size(), 2, 8, 1);
    mi.mouse.cbCreate("BACK2", [path = parent->path, this](const string &btnName){
        if (btnName == "BACK") {
            parent->file = nullptr;
        }
    });
    for (const auto &c : contents) {
        con.println(" " + c);
    }
    mi.sync(3);
    mi.mouse.btnDel(vector<string>{"BACK"});
    mi.mouse.cbClean("BACK");
}

void hnfcOS::Kit::regFile(HNCInterPreter::NodeInfo::FileEntry &f, string &path, int &i, int &indent, vector<string> &objectNames) {
    string objName = f.name + to_string(i);
    objectNames.push_back(objName);
    hncip.script("hnfcOS/display/fileview.chns", "FILE", vector<string>{"FILENAME", "BLOCK"}, vector<string>{f.name, string(indent, ' ')});
    mi.mouse.btnAdd(objName, 1, 4 + i, 30, 3);
    
    auto *ptr = &f;
    
    mi.mouse.cbCreate(objName, [ptr, objName, this, path = path](const string& btnName) mutable { 
        if (btnName == objName) {
            
            string filePath;
            
            // 檔案所在目錄 (File Directory, FD)
            string fileDir = path;
            if (fileDir.back() != '/') fileDir += '/';
            
            // 當前工作目錄 (Current Working Directory, CWD)
            string currentDir = parent->path;
            if (currentDir.back() != '/') currentDir += '/';
            if (currentDir == "") currentDir = "/";

            // 1. FD == CWD: 簡單使用檔名
            if (fileDir == currentDir) {
                filePath = ptr->name;
            } 
            // 2. FD 是 CWD 的父目錄 (需要 ../)
            else if (currentDir.rfind(fileDir, 0) == 0 && currentDir.size() > fileDir.size()) {
                
                // 找出 CWD 相對於 FD 多出來的路徑部分 (e.g., "misc/")
                string relative_segment = currentDir.substr(fileDir.size());
                int depth_diff = 0;

                // 修正後：更穩定的相對深度計算
                if (relative_segment.back() == '/') {
                    relative_segment.pop_back();
                }

                if (!relative_segment.empty()) {
                    depth_diff = 1; // 至少有一個節點
                    for (char c : relative_segment) {
                        if (c == '/') {
                            depth_diff++;
                        }
                    }
                }
                
                // 生成 ../ 前綴
                for (int j = 0; j < depth_diff; ++j) {
                    filePath += "../";
                }
                
                filePath += ptr->name;
            } 
            // 3. 其他情況 (使用絕對路徑回退)
            else {
                if (fileDir == "/" || fileDir.empty()) {
                    filePath = "/" + ptr->name;
                } else {
                    filePath = fileDir + ptr->name;
                }
            }
            
            // 顯示 cat 命令
            parent->termTasks.push_back([filePath = filePath, targetIP = parent->targetIP, path = parent->path](){
                if (!targetIP.empty()) {
                    if (!path.empty()) con.println(targetIP + path + "> cat " + filePath).save();
                    else con.println("@> cat " + filePath).save();
                } else con.println("> cat " + filePath).save();
            });
            
            parent->cmd.Concatenate(filePath, parent->node);
            parent->file = ptr;
        }
    });
    i += 2;
}

void collapseAll(HNCInterPreter::NodeInfo::FolderEntry* f, vector<function<void()>>& tasks, string& path, const string& targetIP) {
    if (!f) return;

    for (auto &sf : f->subfolders) {
        if (sf.expand != 0) {
            collapseAll(&sf, tasks, path, targetIP);
            sf.expand = 0;
        }
    }

    tasks.push_back([targetIP, path]() {
        if (!targetIP.empty()) {
            if (!path.empty()) con.println(targetIP + path + "> cd ..");
            else con.println(targetIP + "@> cd ..");
        } else con.println("> cd ..");
    });

    if (!path.empty()) {
        if (path.back() == '/') path.pop_back();
        size_t pos = path.rfind('/');
        if (pos != string::npos) path.erase(pos + 1);
    }
}


void hnfcOS::Kit::regFolder(HNCInterPreter::NodeInfo::FolderEntry &f, string thisPath, string &path, int &i, int &indent, vector<string> &objectNames, vector<HNCInterPreter::NodeInfo::FolderEntry>* siblings) {
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
                        if (!path.empty()) con.println(targetIP + path + "> cd " + ptr->name).save();
                        else con.println(targetIP + "@> cd " + ptr->name).save();
                    } else con.println("> cd " + ptr->name).save();
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
                        if (!path.empty()) con.println(targetIP + path + "> cd ..").save();
                        else con.println(targetIP + "@> cd ..").save();
                    } else con.println("> cd ..").save();
                });
                if (path.back() == '/') path.pop_back();
                size_t pos = path.rfind('/');
                if (pos != string::npos) path.erase(pos + 1);
            }
        }
    });
    i += 2;

    if (f.expand == 2) {
        thisPath += f.name + "/";
        int childIndent = indent + 1;
        for (auto &sf : f.subfolders) {
            regFolder(sf, thisPath, path, i, childIndent, objectNames, &f.subfolders);
        }
        for (auto &fi : f.files) {
            regFile(fi, thisPath, i, childIndent, objectNames);
        }
    }
}

void hnfcOS::Application::FileView(HNCInterPreter::NodeInfo& node) {
    bool back = false;
    while (!back) {
        #ifdef _WIN32
        con.clear();
        #elif defined(__APPLE__) || defined(__linux__)
        con.clearBuf2();
        #endif
        parent->MenuBar();
        int i = 0, indent = 0;
        vector<string> objectNames;
        hncip.script("hnfcOS/display/fileview.chns", "TOPLINE_" + playerLang, vector<string>{"NODENAME"}, vector<string>{node.Name});
        string thisPath = "/";
        for (auto &f : node.folders) {
            parent->kit.regFolder(f, thisPath, parent->path, i, indent, objectNames, &node.folders);
        }
        for (auto &f : node.files) {
            parent->kit.regFile(f, thisPath, i, indent, objectNames);
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
        if (parent->file) {
            mi.mouse.btnDel(objectNames);
            for (const auto &oN : objectNames) {
                mi.mouse.cbClean(oN);
            }
            mi.mouse.cbCreate("BACK", [&](const string& btnName){
                if (btnName == "EXIT" ||
                    btnName == "SETTINGS" || btnName == "SAVE" ||
                    btnName == "TERMINAL" || btnName == "DISPLAY" ||
                    btnName == "NETMAP" || btnName == "RAM" || btnName == "MAIL") back = true;
            });
            parent->kit.viewFile(parent->file->name, parent->file->contents);
            mi.mouse.cbClean("BACK");
        } else mi.sync(3);
        mi.mouse.btnDel(objectNames);
        for (const auto &oN : objectNames) {
            mi.mouse.cbClean(oN);
        }
    }
}