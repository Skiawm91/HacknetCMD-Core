#include "UI.h"
#include "input.h"
#include "data.h"
#include "console.h"
#include "misc.h"
#include "HNCIP.h"
#include "crypto.h"
#include <iostream>
using namespace std;

extern ManageInput mi;
extern Data dta;
extern Misc misc;
extern Console con;
extern HNCInterPreter hncip;

void UserInterface::SettingsOptions::General(const bool &isPlaying) {
    hncip.script("settings.chns", "VERBOSEBOOTCB_" + to_string(dta.cfg.verboseBoot));
    hncip.script("settings.chns", "VERBOSEBOOTINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "DYNAMICLOGOCB_" + to_string(dta.cfg.dynamicLogo));
    hncip.script("settings.chns", "DYNAMICLOGOINFO_" + misc.toLangName(dta.cfg.language));
    if (isPlaying) cout << "\n\n\n" << flush;
    else {
        hncip.script("settings.chns", "LANGUAGEINFO_" + misc.toLangName(dta.cfg.language));
        hncip.script("settings.chns", "LANGUAGESB_" + to_string(dta.cfg.language));
    }
    hncip.script("settings.chns", "CMDSIZEINFO_" + misc.toLangName(dta.cfg.language));
    hncip.script("settings.chns", "CMDSIZESB_" + to_string(dta.cfg.cmdsize));
    hncip.script("settings.chns", "BACK_" + misc.toLangName(dta.cfg.language));
    mi.mouse.btnAdd("VERBOSEBOOT", 3, 2, 4, 3);
    mi.mouse.btnAdd("DYNAMICLOGO", 3, 5, 4, 3);
    if (!isPlaying) mi.mouse.btnAdd("LANGUAGE", 12, 8, 20, 3);
    mi.mouse.btnAdd("CMDSIZE", 12, 11, 20, 3);
    mi.mouse.btnAdd("BACK", 3, 14, 30, 3);
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
        // EE
        #if defined(__APPLE__) || defined(__linux__)
        if (SHA256Encrypt(mi.kb.getInput()) == "87b3d1fffafad14f7557b3acb40c406682163f21e2dab830d13925ee24478a4b") parent->mode = 1;
        #endif
    } else if (escDetected) {
        escDetected = false;
        parent->back = true;
    }
    mi.mouse.btnDel(vector<string>{"VERBOSEBOOT", "DYNAMICLOGO", "LANGUAGE", "CMDSIZE", "BACK"});
    mi.mouse.cbClean("GENERAL");
}