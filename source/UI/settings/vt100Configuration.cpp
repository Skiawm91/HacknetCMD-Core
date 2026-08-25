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

void UserInterface::Settings(const bool isPlaying) {
    bool back = false;
    while(!back) {
        bool vt100ColorWarn = false;
        bool vt100InputWarn = false;
        con.cursor.hide();
        mi.kb.disable();
        // 重新載入Config
        dta.cfg.reload();
        #if defined(__APPLE__) || defined(__linux__)
        if (isPlaying) con.clearBuf2();
        else con.clear();
        #elif _WIN32
        con.clear();
        #endif
        hncip.script("settings.chns", "TITLE_" + misc.toLangName(dta.cfg.language));
        hncip.script("settings.chns", "VT100COLORINFO_" + misc.toLangName(dta.cfg.language));
        hncip.script("settings.chns", "VT100COLORCB_" + to_string(dta.cfg.vt100Color));
        hncip.script("settings.chns", "BACK_" + misc.toLangName(dta.cfg.language));
        mi.mouse.btnAdd("VT100COLOR", 23, 2, 4, 3);
        mi.mouse.btnAdd("BACK", 1, 5, 30, 3);
        mi.mouse.cbCreate("VT100CONFIGURATION", [&](const string& btnName){
            if (btnName == "VT100COLOR") {
                if (dta.cfg.vt100Color == 1) vt100ColorWarn = true;
                else dta.replace("data/config.hnd", "VT100COLOR=" + to_string(dta.cfg.vt100Color), "VT100COLOR=" + to_string(dta.cfg.vt100Color + 1));
            }
            if (btnName == "BACK") back = true;
        });
        mi.sync(1);
        if (enterDetected) {
            enterDetected = false;
        } else if (escDetected) {
            escDetected = false;
            back = true;
        }
        mi.mouse.btnDel(vector<string>{"VT100COLOR", "BACK"});
        mi.mouse.cbClean("VT100CONFIGURATION");
        if (vt100ColorWarn) {
            int chse;
            #if defined(__APPLE__) || defined(__linux__)
            if (isPlaying) con.clearBuf2();
            else con.clear();
            #elif _WIN32
            con.clear();
            #endif
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
        }
    }
}