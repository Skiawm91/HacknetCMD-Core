#include "UI.h"
#include "input.h"
#include "config.h"
#include "console.h"
#include "misc.h"
#include "hnasm.h"
using namespace std;

extern ManageInput mi;
extern Config cfg;
extern Misc misc;
extern Console con;

void UserInterface::Settings() {
    bool back = false;
    while(!back) {
        mi.kb.enable();
        // 重新載入Config
        cfg.reload();
        con.clear();
        HNASM("settings.chns", "TITLE_" + misc.toLangName(cfg.settings.language));
        HNASM("settings.chns", "VERBOSE_" + to_string(cfg.settings.verbose));
        HNASM("settings.chns", "LANGUAGE_" + to_string(cfg.settings.language));
        HNASM("settings.chns", "CMDSIZE_" + to_string(cfg.settings.cmdsize));
        HNASM("settings.chns", "BACK_" + misc.toLangName(cfg.settings.language));
        mi.mouse.btnAdd("VERBOSE", 1, 1, 20, 3);
        mi.mouse.btnAdd("LANGUAGE", 1, 4, 20, 3);
        mi.mouse.btnAdd("CMDSIZE", 1, 7, 20, 3);
        mi.mouse.btnAdd("BACK", 1, 10, 20, 3);
        mi.mouse.cbCreate([&](const string& btnName){
            if (btnName == "VERBOSE") cfg.data.replace("config/config.hnd", "VERBOSE=" + to_string(cfg.settings.verbose), "VERBOSE=" + to_string(!cfg.settings.verbose));
            if (btnName == "LANGUAGE") {
                if (cfg.settings.language == 2) cfg.data.replace("config/config.hnd", "LANGUAGE=" + to_string(cfg.settings.language), "LANGUAGE=0");
                else cfg.data.replace("config/config.hnd", "LANGUAGE=" + to_string(cfg.settings.language), "LANGUAGE=" + to_string(cfg.settings.language + 1));
            }
            if (btnName == "CMDSIZE") {
                if (cfg.settings.cmdsize == 3) cfg.data.replace("config/config.hnd", "CMDSIZE=" + to_string(cfg.settings.cmdsize), "CMDSIZE=0");
                else cfg.data.replace("config/config.hnd", "CMDSIZE=" + to_string(cfg.settings.cmdsize), "CMDSIZE=" + to_string(cfg.settings.cmdsize + 1));
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
        mi.mouse.btnDel(vector<string>{"VERBOSE", "LANGUAGE", "CMDSIZE" ,"BACK"});
        mi.mouse.cbClean();
    }
}