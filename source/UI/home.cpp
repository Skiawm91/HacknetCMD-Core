#define _HAS_STD_BYTE 0
#include "../function/function.h"
#include "../input/input.h"
#include "../hnasm/hnasm.h"
#include <vector>
module UI;
using namespace std;

extern ManageInput mi;
extern Function func;
extern UserInterface UI;
extern string ver, verStage;

void UserInterface::Home(){
    int chse;
    func.audio.stop();
    func.audio.play("AmbientDroneClipped.wav");
    while(true) {
        mi.kbDisable();
        chse = 0;
        if (verStage != "Release") HNASM("ui.chns", "LOGO", "VER", ver + " [" + verStage + "]");
        else HNASM("ui.chns", "LOGO", "VER", ver);
        HNASM("ui.chns", "HOME");
        mi.btnAdd("PLAY", 2, 8, 30, 3);
        mi.btnAdd("SETTINGS", 2, 14, 30, 3);
        mi.btnAdd("QUIT", 2, 17, 30, 3);
        mi.cbCreate([&](const string& btnName){
            if (btnName == "PLAY") chse = 1;
            if (btnName == "SETTINGS") chse = 3;
            if (btnName == "QUIT") chse = 4;
        });
        mi.async(3);
        mi.btnDel(vector<string>{"PLAY", "SETTINGS", "QUIT"});
        mi.cbClean();
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
                        HNASM("ui.chns", "QUIT");
                        mi.btnAdd("QUIT", 1, 2, 30, 3);
                        mi.btnAdd("CANCEL", 1, 5, 30, 3);
                        mi.cbCreate([&](const string& btnName){
                            if (btnName == "QUIT") chse = 1;
                            if (btnName == "CANCEL") chse = 2;
                        });
                        mi.async(3);
                        mi.btnDel(vector<string>{"QUIT", "CANCEL"});
                        mi.cbClean();
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