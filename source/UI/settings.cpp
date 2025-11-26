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
        #ifdef __APPLE__
        if (isPlaying) con.clearBuf2();
        else con.clear();
        #elif _WIN32
        con.clear();
        #endif
        hncip.script("settings.chns", "TITLE_" + misc.toLangName(dta.cfg.language));
        hncip.script("settings.chns", "VERBOSE_" + to_string(dta.cfg.verbose));
        hncip.script("settings.chns", "LOGO_" + to_string(dta.cfg.logo));
        if (isPlaying) cout << "\n\n\n" << flush;
        else hncip.script("settings.chns", "LANGUAGE_" + to_string(dta.cfg.language));
        hncip.script("settings.chns", "CMDSIZE_" + to_string(dta.cfg.cmdsize));
        hncip.script("settings.chns", "BACK_" + misc.toLangName(dta.cfg.language));
        mi.mouse.btnAdd("VERBOSE", 1, 1, 20, 3);
        mi.mouse.btnAdd("LOGO", 1, 4, 20, 3);
        if (!isPlaying) mi.mouse.btnAdd("LANGUAGE", 1, 7, 20, 3);
        mi.mouse.btnAdd("CMDSIZE", 1, 10, 20, 3);
        mi.mouse.btnAdd("BACK", 1, 13, 20, 3);
        mi.mouse.cbCreate("SETTINGS", [&](const string& btnName){
            if (btnName == "VERBOSE") dta.replace("data/config.hnd", "VERBOSE=" + to_string(dta.cfg.verbose), "VERBOSE=" + to_string(!dta.cfg.verbose));
            if (btnName == "LANGUAGE") {
                if (dta.cfg.language == 2) dta.replace("data/config.hnd", "LANGUAGE=" + to_string(dta.cfg.language), "LANGUAGE=0");
                else dta.replace("data/config.hnd", "LANGUAGE=" + to_string(dta.cfg.language), "LANGUAGE=" + to_string(dta.cfg.language + 1));
            }
            if (btnName == "LOGO") {
                if (dta.cfg.logo == 1) dta.replace("data/config.hnd", "LOGO=" + to_string(dta.cfg.logo), "LOGO=0");
                else dta.replace("data/config.hnd", "LOGO=" + to_string(dta.cfg.logo), "LOGO=" + to_string(dta.cfg.logo + 1));
            }
            if (btnName == "CMDSIZE") {
                if (dta.cfg.cmdsize == 3) dta.replace("data/config.hnd", "CMDSIZE=" + to_string(dta.cfg.cmdsize), "CMDSIZE=0");
                else dta.replace("data/config.hnd", "CMDSIZE=" + to_string(dta.cfg.cmdsize), "CMDSIZE=" + to_string(dta.cfg.cmdsize + 1));
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
        mi.mouse.btnDel(vector<string>{"VERBOSE", "LOGO", "LANGUAGE", "CMDSIZE" ,"BACK"});
        mi.mouse.cbClean("SETTINGS");
    }
}