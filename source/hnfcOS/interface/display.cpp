#define _HAS_STD_BYTE 0
#include "os.h"
#include "input.h"
#include "hnasm.h"
#include "console.h"
#include "config.h"
#include <iostream>
#include <vector>
#include <filesystem>
using namespace std;

extern ManageInput mi;
extern Console son;
extern Config cfg;
extern HNASM hnasm;
extern string playerIP, playerLang;

void hnfcOS::Display() {
    mi.kb.disable();
    cout << "\n\n";
    HNASM::NodeInfo node = getNode();
    if (!node.Name.empty() && !node.Type.empty()) {
        hnasm.script("hnfcOS/display/icon.chns", "ICON_" + node.Type, vector<string>{"TARGETNAME", "TARGETIP"}, vector<string>{node.Name, targetIP});
        hnasm.script("hnfcOS/display/option.chns", "OPTION_" + playerLang);
        mi.mouse.btnAdd("LOGIN", 2, 10, 30, 3);
        mi.mouse.btnAdd("PROBE", 2, 13, 30, 3);
        mi.mouse.btnAdd("FILESYSTEM", 2, 16, 30, 3);
        mi.mouse.btnAdd("LOGS", 2, 19, 30, 3);
        mi.mouse.btnAdd("SCAN", 2, 22, 30, 3);
        mi.mouse.btnAdd("DISCONNECT", 2, 27, 30, 3);
        mi.mouse.cbCreate("DISPLAY", [&](const string& btnName){
            if (btnName == "DISCONNECT") targetIP.clear();
        });
    } else hnasm.script("hnfcOS/display/dced.chns", "DCED_" + playerLang);
    cout.flush();
    mi.async(3);
    mi.mouse.btnDel(vector<string>{"LOGIN", "PROBE", "FILESYSTEM", "LOGS", "SCAN", "DISCONNECT"});
    mi.mouse.cbClean("DISPLAY");
}