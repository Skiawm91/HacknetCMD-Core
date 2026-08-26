#define _HAS_STD_BYTE 0
#include "os.h"
#include "input.h"
#include "HNCIP.h"
#include "console.h"
#include "data.h"
#include <iostream>
#include <vector>
#include <filesystem>
using namespace std;

extern ManageInput mi;
extern Console con;
extern Data dta;
extern HNCInterPreter hncip;
extern string playerIP, playerLang;
extern Data dta;

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
        int endPos = 33 + 3 * dta.cfg.cmdsize;
        hncip.script("hnfcOS/display/option.chns", "OPTION_" + playerLang, vector<string>{"ENDPOS1", "ENDPOS2", "ENDPOS3"}, vector<string>{to_string(endPos), to_string(endPos + 1), to_string(endPos + 2)});
        mi.mouse.btnAdd("LOGIN", 2, 10, 30, 3);
        mi.mouse.btnAdd("PROBE", 2, 13, 30, 3);
        mi.mouse.btnAdd("FILESYSTEM", 2, 16, 30, 3);
        mi.mouse.btnAdd("LOGS", 2, 19, 30, 3);
        mi.mouse.btnAdd("SCAN", 2, 22, 30, 3);
        mi.mouse.btnAdd("DISCONNECT", 2, 33 + 3 * dta.cfg.cmdsize, 30, 3);
        mi.mouse.cbCreate("DISPLAY", [&](const string& btnName){
            if (btnName == "PROBE") displayChse = 2;
            if (btnName == "FILESYSTEM") {
                termTasks.push_back([targetIP = targetIP, path = path, this](){
                    if (!targetIP.empty()) {
                        if (!path.empty()) con.println(targetIP + path + "> ls").save();
                        else con.println(targetIP + "@> ls").save();
                    } else con.println("> ls").save();
                    this->cmd.ListDir(this->node);
                });
                displayChse = 3;
            }
            if (btnName == "LOGS") {
                cmd.ChangeDir(node, "/log");
                termTasks.push_back([targetIP = targetIP, path = path, this](){
                    if (!targetIP.empty()) {
                        if (!path.empty()) con.println(targetIP + path + "> cd /log").save();
                        else con.println(targetIP + "@> cd /log").save();
                    } else con.println("> cd /log").save();
                    this->cmd.ListDir(this->node);
                });
                displayChse = 3;
            }
            if (btnName == "DISCONNECT") {
                termTasks.push_back([targetIP = targetIP, path = path](){
                    if (!targetIP.empty()) {
                        if (!path.empty()) con.println(targetIP + path + "> disconnect").save();
                        else con.println(targetIP + "@> disconnect").save();
                    } else con.println("> disconnect").save();
                    con.println("Disconnected").save();
                });
                sys.cleanNode();
            }
        });
    } else hncip.script("hnfcOS/display/dced.chns", "DCED_" + playerLang);
    cout.flush();
    mi.sync(3);
    mi.mouse.btnDel(vector<string>{"LOGIN", "PROBE", "FILESYSTEM", "LOGS", "SCAN", "DISCONNECT"});
    mi.mouse.cbClean("DISPLAY");
}