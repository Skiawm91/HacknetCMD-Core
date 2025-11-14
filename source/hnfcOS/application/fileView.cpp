#define _HAS_STD_BYTE 0
#include "os.h"
#include "console.h"
#include "HNCIP.h"
#include "input.h"
#include <vector>
#include <string>
using namespace std;

extern Console con;
extern HNCInterPreter hncip;
extern ManageInput mi;
extern string playerLang;

static void regFile(HNCInterPreter::NodeInfo::FileEntry &f, string &path, int &i, int &indent, vector<string> &objectNames) {
    // string objName = f.name + to_string(i);
    // objectNames.push_back(objName);
    hncip.script("hnfcOS/display/fileview.chns", "FILE", vector<string>{"FILENAME", "BLOCK"}, vector<string>{f.name, string(indent, ' ')});
    // mi.mouse.btnAdd(objName, 1, 4 + i, 30, 3);
    // auto *ptr = &f;
    // mi.mouse.cbCreate(objName, [ptr, objName](const string& btnName){});
    i += 2;
}

static void regFolder(HNCInterPreter::NodeInfo::FolderEntry &f, string &path, int &i, int &indent, vector<string> &objectNames, vector<HNCInterPreter::NodeInfo::FolderEntry>* siblings = nullptr) {
    string objName = f.name + to_string(i);
    objectNames.push_back(objName);
    hncip.script("hnfcOS/display/fileview.chns", "FOLDER_" + to_string(f.expand), vector<string>{"FOLDERNAME", "BLOCK"}, vector<string>{f.name, string(indent, ' ')});
    mi.mouse.btnAdd(objName, 1, 4 + i, 30, 3);

    auto *ptr = &f;
    bool expanded;
    mi.mouse.cbCreate(objName, [ptr, objName, &path, siblings, expanded](const string& btnName){
        if (btnName == objName) {
            if (ptr->expand == 0) ptr->expand = 1;
            else ptr->expand = 0;
            if (ptr->expand == 1) {
                if (siblings) {
                    for (auto &sibling : *siblings) {
                        if (&sibling != ptr) {
                            if (sibling.expand != 0) {
                                sibling.expand = 0;
                                if (path.back() == '/') path.pop_back();
                                size_t pos = path.rfind('/');
                                if (pos != string::npos) path.erase(pos + 1);
                            }
                        }
                    }
                }
                path = (path.empty() ? "/" : path) + ptr->name + "/";
                ptr->expand = 2;
            } else if (ptr->expand == 0) {
                for (auto &sf : ptr->subfolders) {
                    if (sf.expand != 0) {
                        sf.expand = 0;
                        if (path.back() == '/') path.pop_back();
                        size_t pos = path.rfind('/');
                        if (pos != string::npos) path.erase(pos + 1);
                    }
                }
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
            regFolder(f, parent->path, i, indent, objectNames, &node.folders);
        }
        objectNames.push_back("BACK");
        hncip.script("hnfcOS/display/fileview.chns", "BACK");
        mi.mouse.btnAdd("BACK", 1, 4 + i, 30, 3);
        mi.mouse.cbCreate("BACK", [&](const string& btnName){
            if (btnName == "BACK" || btnName == "EXIT" ||
                btnName == "SETTINGS" || btnName == "SAVE" ||
                btnName == "TERMINAL" || btnName == "DISPLAY" ||
                btnName == "NETMAP" || btnName == "RAM" || btnName == "MAIL") back = true;
        });
        mi.async(3);
        mi.mouse.btnDel(objectNames);
        for (const auto &oN : objectNames) {
            mi.mouse.cbClean(oN);
        }
    }
}