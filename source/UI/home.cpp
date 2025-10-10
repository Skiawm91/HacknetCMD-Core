#define _HAS_STD_BYTE 0
#include "UI.h"
#include "function.h"
#include "config.h"
#include "input.h"
#include "HNCIP.h"
#include "misc.h"
#include <vector>
using namespace std;

extern ManageInput mi;
extern Function func;
extern Config cfg;
extern UserInterface UI;
extern Misc misc;
extern string ver, verStage;
extern HNCInterPreter hncip;

void UserInterface::Home(){
    int chse;
    func.audio.stop();
    func.audio.play("AmbientDroneClipped.wav");
    while(true) {
        mi.kb.disable();
        chse = 0;
        if (verStage != "Release") hncip.script("ui.chns", "LOGO", vector<string>{"VER"}, vector<string>{ver + " [" + verStage + "]"});
        else hncip.script("ui.chns", "LOGO", vector<string>{"VER"}, vector<string>{ver});
        hncip.script("ui.chns", "HOME");
        mi.mouse.btnAdd("PLAY", 2, 8, 30, 3);
        mi.mouse.btnAdd("SETTINGS", 2, 14, 30, 3);
        mi.mouse.btnAdd("QUIT", 2, 17, 30, 3);
        mi.mouse.cbCreate("HOME", [&](const string& btnName){
            if (btnName == "PLAY") chse = 1;
            if (btnName == "SETTINGS") chse = 3;
            if (btnName == "QUIT") chse = 4;
        });
        mi.async(3);
        mi.mouse.btnDel(vector<string>{"PLAY", "SETTINGS", "QUIT"});
        mi.mouse.cbClean();
        switch(chse) {
            case 1:
                UI.Login();
                break;
            case 3:
                UI.Settings();
                break;
            case 4:
                {
                    while(true) {
                        chse = 0;
                        hncip.script("ui.chns", "QUIT_" + misc.toLangName(cfg.settings.language));
                        mi.mouse.btnAdd("QUIT", 1, 2, 30, 3);
                        mi.mouse.btnAdd("CANCEL", 1, 5, 30, 3);
                        mi.mouse.cbCreate("QUIT", [&](const string& btnName){
                            if (btnName == "QUIT") chse = 1;
                            if (btnName == "CANCEL") chse = 2;
                        });
                        mi.async(3);
                        mi.mouse.btnDel(vector<string>{"QUIT", "CANCEL"});
                        mi.mouse.cbClean();
                        if (chse == 1) {
                            return;
                        } else if (chse == 2) {
                            break;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
}