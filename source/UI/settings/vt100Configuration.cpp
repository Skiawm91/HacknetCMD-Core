#include "UI.h"
#include "input.h"
#include "data.h"
#include "console.h"
#include "misc.h"
#include "HNCIP.h"
#include <iostream>
using namespace std;

extern ManageInput mi;
extern Data dta;
extern Misc misc;
extern Console con;
extern HNCInterPreter hncip;

void UserInterface::SettingsOptions::VT100Configuration() {
    bool vt100ColorWarn = false;
    bool vt100CMDResizeWarn = false;
    hncip.script("settings.chns", "VT100COLORINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "VT100COLORCB_" + to_string(dta.cfg.vt100Color));
    hncip.script("settings.chns", "VT100CMDRESIZEINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "VT100CMDRESIZECB_" + to_string(dta.cfg.vt100Resize));
    hncip.script("settings.chns", "BACK_" + misc.toLangName(dta.cfg.language));
    mi.mouse.btnAdd("VT100COLOR", 23, 2, 4, 3);
    mi.mouse.btnAdd("VT100CMDRESIZE", 21, 5, 4, 3);
    mi.mouse.btnAdd("BACK", 1, 8, 30, 3);
    mi.mouse.cbCreate("VT100CONFIGURATION", [&](const string& btnName){
        if (btnName == "VT100COLOR") {
            if (dta.cfg.vt100Color == 1) vt100ColorWarn = true;
            else dta.replace("data/config.hnd", "VT100COLOR=" + to_string(dta.cfg.vt100Color), "VT100COLOR=" + to_string(dta.cfg.vt100Color + 1));
        }
        if (btnName == "VT100CMDRESIZE") {
            if (dta.cfg.vt100Resize == 1) vt100CMDResizeWarn = true;
            else dta.replace("data/config.hnd", "VT100CMDRESIZE=" + to_string(dta.cfg.vt100Resize), "VT100CMDRESIZE=" + to_string(dta.cfg.vt100Resize + 1));
        }
        if (btnName == "BACK") parent->back = true;
    });
    mi.sync(1);
    if (enterDetected) {
        enterDetected = false;
    } else if (escDetected) {
        escDetected = false;
        parent->back = true;
    }
    mi.mouse.btnDel(vector<string>{"VT100COLOR", "VT100CMDRESIZE", "BACK"});
    mi.mouse.cbClean("VT100CONFIGURATION");
    if (vt100ColorWarn) {
        int chse;
        con.clear();
        hncip.script("settings.chns", "VT100COLORWARN_" + misc.toLangName(dta.cfg.language));
        mi.mouse.btnAdd("CONFIRM", 1, 4, 30, 3);
        mi.mouse.btnAdd("CANCEL", 1, 7, 30, 3);
        mi.mouse.cbCreate("VT100COLORWARN", [&](const string& btnName){
            if (btnName == "CONFIRM") chse = 1;
            if (btnName == "CANCEL") chse = 2;
        });
        mi.sync(1);
        if (enterDetected) {
            enterDetected = false;
            chse = 1;
        } else if (escDetected) {
            escDetected = false;
            chse = 2;
        }
        if (chse == 1) dta.replace("data/config.hnd", "VT100COLOR=" + to_string(dta.cfg.vt100Color), "VT100COLOR=0");
        mi.mouse.btnDel(vector<string>{"CONFIRM", "CANCEL"});
        mi.mouse.cbClean("VT100COLORWARN");
    } else if (vt100CMDResizeWarn) {
        int chse;
        con.clear();
        hncip.script("settings.chns", "VT100CMDRESIZEWARN_" + misc.toLangName(dta.cfg.language));
        mi.mouse.btnAdd("CONFIRM", 1, 4, 30, 3);
        mi.mouse.btnAdd("CANCEL", 1, 7, 30, 3);
        mi.mouse.cbCreate("VT100CMDRESIZEWARN", [&](const string& btnName){
            if (btnName == "CONFIRM") chse = 1;
            if (btnName == "CANCEL") chse = 2;
        });
        mi.sync(1);
        if (enterDetected) {
            enterDetected = false;
            chse = 1;
        } else if (escDetected) {
            escDetected = false;
            chse = 2;
        }
        if (chse == 1) dta.replace("data/config.hnd", "VT100CMDRESIZE=" + to_string(dta.cfg.vt100Resize), "VT100CMDRESIZE=0");
        mi.mouse.btnDel(vector<string>{"CONFIRM", "CANCEL"});
        mi.mouse.cbClean("VT100CMDRESIZEWARN");
    }
}