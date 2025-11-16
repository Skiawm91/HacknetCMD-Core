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

void hnfcOS::Display() {
    Mode = "Display";
    con.cursor.hide();
    mi.kb.disable();
    cout << "\n\n";
    if (displayChse != 0) {
        if (displayChse == 2) app.Probe(node);
        else if (displayChse == 3) app.FileView(node);
        return;
    }
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
            if (btnName == "PROBE") displayChse = 2;
            if (btnName == "FILESYSTEM") displayChse = 3;
            // if (btnName == "LOGS") chse = 4;
            if (btnName == "DISCONNECT") {
                termTasks.push_back([targetIP = targetIP, path = path](){
                    if (!targetIP.empty()) {
                        if (!path.empty()) std::cout << targetIP << path << "> disconnect" << std::endl;
                        else std::cout << targetIP << "@> disconnect" << std::endl;
                    } else cout << "> disconnect" << std::endl;
                    std::cout << "Disconnected" << std::endl;
                });
                targetIP.clear();
                node = getNode(targetIP);
                path.clear();
                displayChse = 0;
            }
        });
    } else hncip.script("hnfcOS/display/dced.chns", "DCED_" + playerLang);
    cout.flush();
    mi.async(3);
    mi.mouse.btnDel(vector<string>{"LOGIN", "PROBE", "FILESYSTEM", "LOGS", "SCAN", "DISCONNECT"});
    mi.mouse.cbClean("DISPLAY");
}