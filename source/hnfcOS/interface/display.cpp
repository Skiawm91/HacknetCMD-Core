#define _HAS_STD_BYTE 0
#include "os.h"
#include "input.h"
#include "HNCIP.h"
#include "console.h"
#include "config.h"
#include <iostream>
#include <vector>
#include <filesystem>
using namespace std;

extern ManageInput mi;
extern Console con;
extern Config cfg;
extern HNCInterPreter hncip;
extern string playerIP, playerLang;

void hnfcOS::Application::Probe(HNCInterPreter::NodeInfo& node) {
    #ifdef _WIN32
    con.clear();
    #elif __APPLE__
    con.clearBuf2();
    #endif
    parent->MenuBar();
    int i = 0;
    hncip.script("hnfcOS/display/probe.chns", "TOPLINE", vector<string>{"NODENAME", "NODEIP", "PORTS"}, vector<string>{node.Name, node.IP, to_string(node.Ports)});
    for (const auto &pN : node.portNames) {
        if (pN == "SSH") hncip.script("hnfcOS/display/probe.chns", "SSH", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "FTP") hncip.script("hnfcOS/display/probe.chns", "FTP", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "HTTP") hncip.script("hnfcOS/display/probe.chns", "HTTP", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "SMTP") hncip.script("hnfcOS/display/probe.chns", "SMTP", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "SSL") hncip.script("hnfcOS/display/probe.chns", "SSL", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "SQL") hncip.script("hnfcOS/display/probe.chns", "SQL", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        i++;
    }
    mi.async(3);
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
        vector<string> objects;
        hncip.script("hnfcOS/display/fileview.chns", "TOPLINE_" + playerLang, vector<string>{"NODENAME"}, vector<string>{node.Name});
        for (auto &f : node.folders) {
            objects.push_back(f.name);
            hncip.script("hnfcOS/display/fileview.chns", "FOLDER_" + to_string(f.expand), vector<string>{"FOLDERNAME"}, vector<string>{f.name});
            if (f.expand) {
                for (auto &fi : f.files) {
                  hncip.script("hnfcOS/display/fileview.chns", "FILE", vector<string>{"FILENAME"}, vector<string>{fi.name});  
                }
                for (auto &sf : f.subfolders) {
                  hncip.script("hnfcOS/display/fileview.chns", "FOLDER_" + to_string(f.expand), vector<string>{"FOLDERNAME"}, vector<string>{sf.name});  
                }
            }
            mi.mouse.btnAdd(f.name, 1, 4 + i, 30, 3);
            auto *ptr = &f;
            mi.mouse.cbCreate(f.name, [ptr](const string& btnName){
                if (btnName == ptr->name) ptr->expand = !ptr->expand;
            });
            i += 3;
        }
        objects.push_back("BACK");
        hncip.script("hnfcOS/display/fileview.chns", "BACK");
        mi.mouse.btnAdd("BACK", 1, 3 + i, 30, 3);
        mi.mouse.cbCreate("BACK", [&](const string& btnName){
            if (btnName == "BACK") back = true;
        });
        mi.async(3);
        mi.mouse.btnDel(objects);
        for (const auto &o : objects) {
            mi.mouse.cbClean(o);
        }
    }
}

void hnfcOS::Display() {
    con.cursor.hide();
    mi.kb.disable();
    int chse = 0;
    cout << "\n\n";
    HNCInterPreter::NodeInfo node = getNode();
    if (!node.Name.empty() && !node.Type.empty()) {
        hncip.script("hnfcOS/display/icon.chns", "ICON_" + node.Type, vector<string>{"TARGETNAME", "TARGETIP"}, vector<string>{node.Name, targetIP});
        hncip.script("hnfcOS/display/option.chns", "OPTION_" + playerLang);
        mi.mouse.btnAdd("LOGIN", 2, 10, 30, 3);
        mi.mouse.btnAdd("PROBE", 2, 13, 30, 3);
        mi.mouse.btnAdd("FILESYSTEM", 2, 16, 30, 3);
        mi.mouse.btnAdd("LOGS", 2, 19, 30, 3);
        mi.mouse.btnAdd("SCAN", 2, 22, 30, 3);
        mi.mouse.btnAdd("DISCONNECT", 2, 27, 30, 3);
        mi.mouse.cbCreate("DISPLAY", [&](const string& btnName){
            if (btnName == "PROBE") chse = 2;
            if (btnName == "FILESYSTEM") chse = 3;
            if (btnName == "DISCONNECT") targetIP.clear();
        });
    } else hncip.script("hnfcOS/display/dced.chns", "DCED_" + playerLang);
    cout.flush();
    mi.async(3);
    mi.mouse.btnDel(vector<string>{"LOGIN", "PROBE", "FILESYSTEM", "LOGS", "SCAN", "DISCONNECT"});
    mi.mouse.cbClean("DISPLAY");
    if (chse == 2) app.Probe(node);
    else if (chse == 3) app.FileView(node);
}