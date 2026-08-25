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
    con.cursor.hide();
    mi.kb.enable();
    inputAte = true; // 防止在設定頁面中看到文字被輸入
    int mode = 0; // General
    mi.mouse.btnAdd("GENERAL", 15, 0, 9, 1);
    #ifdef _WIN32
    mi.mouse.btnAdd("VT100CONFIGURATION", 25, 0, 21, 1);
    #endif
    mi.mouse.cbCreate("SETTINGSMENU", [&](const string& btnName){
        if (btnName == "GENERAL") mode = 0;
        if (btnName == "VT100CONFIGURATION") mode = 1;
    });
    back = false;
    while(!back) {
        // 重新載入Config
        dta.cfg.reload();
        #if defined(__APPLE__) || defined(__linux__)
        if (isPlaying) con.clearBuf2();
        else con.clear();
        #elif _WIN32
        con.clear();
        #endif
        #ifdef _WIN32
        hncip.script("settings.chns", "MENU_" + misc.toLangName(dta.cfg.language));
        #else
        hncip.script("settings.chns", "MENUNOVT100_" + misc.toLangName(dta.cfg.language));
        #endif
        if (mode == 0) settings.General(isPlaying);
        else if (mode == 1) settings.VT100Configuration();
    }
    mi.mouse.btnDel(vector<string>{"GENERAL", "VT100CONFIGURATION"});
    mi.mouse.cbClean("SETTINGSMENU");
}