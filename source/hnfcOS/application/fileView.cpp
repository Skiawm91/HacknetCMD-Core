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

static void regFile(HNCInterPreter::NodeInfo::FileEntry &f, int &i, vector<string> &objectNames) {
    // string objName = f.name + to_string(i);
    // objectNames.push_back(objName);
    hncip.script("hnfcOS/display/fileview.chns", "FILE", vector<string>{"FILENAME"}, vector<string>{f.name});
    // mi.mouse.btnAdd(objName, 1, 4 + i, 30, 3);
    // auto *ptr = &f;
    // mi.mouse.cbCreate(objName, [ptr, objName](const string& btnName){});
    i += 2;
}

static void regFolder(HNCInterPreter::NodeInfo::FolderEntry &f, int &i, vector<string> &objectNames) {
    string objName = f.name + to_string(i);
    objectNames.push_back(objName);
    hncip.script("hnfcOS/display/fileview.chns", "FOLDER_" + to_string(f.expand), vector<string>{"FOLDERNAME"}, vector<string>{f.name});
    mi.mouse.btnAdd(objName, 1, 4 + i, 30, 3);
    auto *ptr = &f;
    mi.mouse.cbCreate(objName, [ptr, objName](const string& btnName){
        if (btnName == objName) ptr->expand = !ptr->expand;
    });
    i += 2;
    if (f.expand) {
        for (auto &sf : f.subfolders) {
            regFolder(sf, i, objectNames);
        }
        for (auto &fi : f.files) {
            regFile(fi, i, objectNames);
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
        int i = 0;
        vector<string> objectNames;
        hncip.script("hnfcOS/display/fileview.chns", "TOPLINE_" + playerLang, vector<string>{"NODENAME"}, vector<string>{node.Name});
        // fs start
        for (auto &f : node.folders) {
            regFolder(f, i, objectNames);
        }
        objectNames.push_back("BACK");
        hncip.script("hnfcOS/display/fileview.chns", "BACK");
        mi.mouse.btnAdd("BACK", 1, 4 + i, 30, 3);
        mi.mouse.cbCreate("BACK", [&](const string& btnName){
            if (btnName == "BACK") back = true;
        });
        mi.async(3);
        mi.mouse.btnDel(objectNames);
        for (const auto &oN : objectNames) {
            mi.mouse.cbClean(oN);
        }
    }
}