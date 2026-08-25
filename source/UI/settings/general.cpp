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

void UserInterface::SettingsOptions::General(const bool &isPlaying) {
    hncip.script("settings.chns", "VERBOSEBOOTINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "VERBOSEBOOTCB_" + to_string(dta.cfg.verboseBoot));
    hncip.script("settings.chns", "DYNAMICLOGOINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "DYNAMICLOGOCB_" + to_string(dta.cfg.dynamicLogo));
    if (isPlaying) cout << "\n\n\n" << flush;
    else {
        hncip.script("settings.chns", "LANGUAGEINFO_" + misc.toLangName(dta.cfg.language));
        hncip.script("settings.chns", "LANGUAGESB_" + to_string(dta.cfg.language));
    }
    hncip.script("settings.chns", "CMDSIZEINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "CMDSIZESB_" + to_string(dta.cfg.cmdsize));
    hncip.script("settings.chns", "BACK_" + misc.toLangName(dta.cfg.language));
    mi.mouse.btnAdd("VERBOSEBOOT", 16, 2, 4, 3);
    mi.mouse.btnAdd("DYNAMICLOGO", 16, 5, 4, 3);
    if (!isPlaying) mi.mouse.btnAdd("LANGUAGE", 12, 8, 20, 3);
    mi.mouse.btnAdd("CMDSIZE", 12, 11, 20, 3);
    mi.mouse.btnAdd("BACK", 1, 14, 30, 3);
    mi.mouse.cbCreate("GENERAL", [&](const string& btnName){
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
        if (btnName == "BACK") parent->back = true;
    });
    mi.sync(1);
    if (enterDetected) {
        enterDetected = false;
    } else if (escDetected) {
        escDetected = false;
        parent->back = true;
    }
    mi.mouse.btnDel(vector<string>{"VERBOSEBOOT", "DYNAMICLOGO", "LANGUAGE", "CMDSIZE", "BACK"});
    mi.mouse.cbClean("GENERAL");
}