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
        hncip.script("settings.chns", "VERBOSEBOOTINFO_" + misc.toLangName(dta.cfg.language));
        hncip.script("settings.chns", "VERBOSEBOOTCB_" + to_string(dta.cfg.verboseBoot));
        hncip.script("settings.chns", "DYNAMICLOGOINFO_" + misc.toLangName(dta.cfg.language));
        hncip.script("settings.chns", "DYNAMICLOGOCB_" + to_string(dta.cfg.dynamicLogo));
        if (isPlaying) cout << "\n\n\n" << flush;
        else hncip.script("settings.chns", "LANGUAGE_" + to_string(dta.cfg.language));
        hncip.script("settings.chns", "CMDSIZE_" + to_string(dta.cfg.cmdsize));
        #ifdef _WIN32
            hncip.script("settings.chns", "VT100COLOR_" + to_string(dta.cfg.vt100color));
        #endif
        hncip.script("settings.chns", "BACK_" + misc.toLangName(dta.cfg.language));
        mi.mouse.btnAdd("VERBOSEBOOT", 16, 2, 4, 3);
        mi.mouse.btnAdd("DYNAMICLOGO", 16, 5, 4, 3);
        if (!isPlaying) mi.mouse.btnAdd("LANGUAGE", 1, 7, 20, 3);
        mi.mouse.btnAdd("CMDSIZE", 1, 10, 20, 3);
        #ifdef _WIN32
            mi.mouse.btnAdd("VT100COLOR", 1, 13, 20, 3);
            mi.mouse.btnAdd("BACK", 1, 16, 20, 3);
        #else
            mi.mouse.btnAdd("BACK", 1, 13, 20, 3);
        #endif
        mi.mouse.cbCreate("SETTINGS", [&](const string& btnName){
            if (btnName == "VERBOSEBOOT") dta.replace("data/config.hnd", "VERBOSEBOOT=" + to_string(dta.cfg.verboseBoot), "VERBOSEBOOT=" + to_string(!dta.cfg.verboseBoot));
            if (btnName == "LANGUAGE") {
                if (dta.cfg.language == 2) dta.replace("data/config.hnd", "LANGUAGE=" + to_string(dta.cfg.language), "LANGUAGE=0");
                else dta.replace("data/config.hnd", "LANGUAGE=" + to_string(dta.cfg.language), "LANGUAGE=" + to_string(dta.cfg.language + 1));
            }
            if (btnName == "DYNAMICLOGO") {
                if (dta.cfg.dynamicLogo == 1) dta.replace("data/config.hnd", "DYNAMICLOGO=" + to_string(dta.cfg.dynamicLogo), "DYNAMICLOGO=0");
                else dta.replace("data/config.hnd", "DYNAMICLOGO=" + to_string(dta.cfg.dynamicLogo), "DYNAMICLOGO=" + to_string(dta.cfg.dynamicLogo + 1));
            }
            if (btnName == "CMDSIZE") {
                if (dta.cfg.cmdsize == 3) dta.replace("data/config.hnd", "CMDSIZE=" + to_string(dta.cfg.cmdsize), "CMDSIZE=0");
                else dta.replace("data/config.hnd", "CMDSIZE=" + to_string(dta.cfg.cmdsize), "CMDSIZE=" + to_string(dta.cfg.cmdsize + 1));
            }
            if (btnName == "VT100COLOR") {
                if (dta.cfg.vt100color == 1) dta.replace("data/config.hnd", "VT100COLOR=" + to_string(dta.cfg.vt100color), "VT100COLOR=0");
                else dta.replace("data/config.hnd", "VT100COLOR=" + to_string(dta.cfg.vt100color), "VT100COLOR=" + to_string(dta.cfg.vt100color + 1));
            }
            if (btnName == "BACK") back = true;
        });
        mi.async(1);
        if (enterDetected) {
            enterDetected = false;
        } else if (escDetected) {
            escDetected = false;
            back = true;
        }
        mi.mouse.btnDel(vector<string>{"VERBOSEBOOT", "DYNAMICLOGO", "LANGUAGE", "CMDSIZE", "VT100COLOR", "BACK"});
        mi.mouse.cbClean("SETTINGS");
    }
}