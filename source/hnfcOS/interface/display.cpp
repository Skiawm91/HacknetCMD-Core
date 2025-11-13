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

void hnfcOS::Display() {
    con.cursor.hide();
    mi.kb.disable();
    static int chse;
    cout << "\n\n";
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
            if (btnName == "LOGS") chse = 4;
            if (btnName == "DISCONNECT") targetIP.clear();
        });
    } else hncip.script("hnfcOS/display/dced.chns", "DCED_" + playerLang);
    cout.flush();
    mi.async(3);
    mi.mouse.btnDel(vector<string>{"LOGIN", "PROBE", "FILESYSTEM", "LOGS", "SCAN", "DISCONNECT"});
    mi.mouse.cbClean("DISPLAY");
    if (chse == 2) app.Probe(node);
    else if (chse == 3) app.FileView(node);
    else if (chse == 4) {
        for (auto &f : node.folders)
            if (f.name == "log") f.expand = true;
        app.FileView(node);
        chse = 0;
    }
}