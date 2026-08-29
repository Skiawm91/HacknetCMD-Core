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
    #ifdef _WIN32
    bool vt100ColorWarn = false;
    bool vt100CMDResizeWarn = false;
    hncip.script("settings.chns", "VT100COLORCB_" + to_string(dta.cfg.vt100Color));
    hncip.script("settings.chns", "VT100COLORINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "VT100CMDRESIZECB_" + to_string(dta.cfg.vt100Resize));
    hncip.script("settings.chns", "VT100CMDRESIZEINFO_" + misc.toLangName(dta.cfg.language));
    #elif defined(__APPLE__) || defined(__linux__)
    bool true24BitColorWarn = false;
    hncip.script("settings.chns", "TRUE24BITCOLORCB_" + to_string(dta.cfg.true24BitColor));
    hncip.script("settings.chns", "TRUE24BITCOLORINFO_" + misc.toLangName(dta.cfg.language));
    #endif
    hncip.script("settings.chns", "BACK_" + misc.toLangName(dta.cfg.language));
    #ifdef _WIN32
    mi.mouse.btnAdd("VT100COLOR", 3, 2, 4, 3);
    mi.mouse.btnAdd("VT100CMDRESIZE", 3, 5, 4, 3);
    mi.mouse.btnAdd("BACK", 3, 8, 30, 3);
    #elif defined(__APPLE__) || defined(__linux__)
    mi.mouse.btnAdd("TRUE24BITCOLOR", 3, 2, 4, 3);
mi.mouse.btnAdd("BACK", 3, 5, 30, 3);
    #endif
    mi.mouse.cbCreate("VT100CONFIGURATION", [&](const string& btnName){
        #ifdef _WIN32
        if (btnName == "VT100COLOR") {
            if (dta.cfg.vt100Color == 1) vt100ColorWarn = true;
            else dta.replace("data/config.hnd", "VT100COLOR=" + to_string(dta.cfg.vt100Color), "VT100COLOR=" + to_string(dta.cfg.vt100Color + 1));
        }
        if (btnName == "VT100CMDRESIZE") {
            if (dta.cfg.vt100Resize == 1) vt100CMDResizeWarn = true;
            else dta.replace("data/config.hnd", "VT100CMDRESIZE=" + to_string(dta.cfg.vt100Resize), "VT100CMDRESIZE=" + to_string(dta.cfg.vt100Resize + 1));
        }
        #elif defined(__APPLE__) || defined(__linux__)
        if (btnName == "TRUE24BITCOLOR") {
            if (dta.cfg.true24BitColor == 1) true24BitColorWarn = true;
            else dta.replace("data/config.hnd", "TRUE24BITCOLOR=" + to_string(dta.cfg.true24BitColor), "TRUE24BITCOLOR=" + to_string(dta.cfg.true24BitColor + 1));
        }
        #endif
        if (btnName == "BACK") parent->back = true;
    });
    mi.sync(1);
    if (enterDetected) {
        enterDetected = false;
    } else if (escDetected) {
        escDetected = false;
        parent->back = true;
    }
    mi.mouse.btnDel(vector<string>{"VT100COLOR", "VT100CMDRESIZE", "TRUE24BITCOLOR", "BACK"});
    mi.mouse.cbClean("VT100CONFIGURATION");
    #ifdef _WIN32
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
    #elif defined(__APPLE__) || defined(__linux__)
    if (true24BitColorWarn) {
        int chse;
        con.clear();
        hncip.script("settings.chns", "TRUE24BITCOLORWARN_" + misc.toLangName(dta.cfg.language));
        mi.mouse.btnAdd("CONFIRM", 1, 4, 30, 3);
        mi.mouse.btnAdd("CANCEL", 1, 7, 30, 3);
        mi.mouse.cbCreate("TRUE24BITCOLORWARN", [&](const string& btnName){
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
        if (chse == 1) dta.replace("data/config.hnd", "TRUE24BITCOLOR=" + to_string(dta.cfg.true24BitColor), "TRUE24BITCOLOR=0");
        mi.mouse.btnDel(vector<string>{"CONFIRM", "CANCEL"});
        mi.mouse.cbClean("TRUE24BITCOLORWARN");
    }
    #endif
}