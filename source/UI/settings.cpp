#include "UI.h"
#include "../input/input.h"
#include "../config/config.h"
#include "../function/function.h"
#include "../hnasm/hnasm.h"
using namespace std;

extern ManageInput mi;
extern Config cfg;
extern Function func;

void UserInterface::Settings() {
    bool back = false;
    while(!back) {
        func.cmd.clear();
        mi.kbEnable();
        // 重新載入Config
        cfg.reload();
        HNASM("settings.chns", "VERBOSE_" + to_string(cfg.settings.verbose));
        HNASM("settings.chns", "LANGUAGE_" + to_string(cfg.settings.language));
        HNASM("settings.chns", "BACK");
        mi.btnAdd("VERBOSE", 0, 1, 20, 3);
        mi.btnAdd("LANGUAGE", 0, 4, 20, 3);
        mi.btnAdd("BACK", 0, 7, 20, 3);
        mi.cbCreate([&](const string& btnName){
            if (btnName == "VERBOSE") cfg.data.replace("config/config.hnd", "VERBOSE=" + to_string(cfg.settings.verbose), "VERBOSE=" + to_string(!cfg.settings.verbose));
            if (btnName == "LANGUAGE") {
                if (cfg.settings.language == 2) cfg.data.replace("config/config.hnd", "LANGUAGE=" + to_string(cfg.settings.language), "LANGUAGE=0");
                else cfg.data.replace("config/config.hnd", "LANGUAGE=" + to_string(cfg.settings.language), "LANGUAGE=" + to_string(cfg.settings.language + 1));
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
        mi.btnDel(vector<string>{"VERBOSE", "LANGUAGE" ,"BACK"});
        mi.cbClean();
    }
}