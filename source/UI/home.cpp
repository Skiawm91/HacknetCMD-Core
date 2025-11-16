#define _HAS_STD_BYTE 0
#include "UI.h"
#include "function.h"
#include "config.h"
#include "input.h"
#include "HNCIP.h"
#include "misc.h"
#include "console.h"
#include "discord_rpc.h"
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
using namespace std;

extern ManageInput mi;
extern Function func;
extern Config cfg;
extern UserInterface UI;
extern Misc misc;
extern string ver, verStage;
extern HNCInterPreter hncip;
extern DiscordRichPresence drp;
extern Console con;

void UserInterface::Home(){
    int chse;
    atomic<bool> running, logoAnimation;
    func.audio.stop();
    func.audio.playL("ADC", vector<string>{"AmbientDroneClipped.wav"});
    while(true) {
        con.clear();
        con.cursor.hide();
        drp.state = "Home";
        Discord_UpdatePresence(&drp);
        mi.kb.disable();
        chse = 0;
        hncip.script("ui.chns", "HOME");
        if (cfg.settings.logo == 1) {
            logoAnimation = true;
            thread([&]{
                running = true;
                int fps = 30, frame = 1;
                while(logoAnimation) {
                    if (frame > 60) frame = 1;
                    if (verStage != "Release") hncip.script("logo.chns", "LOGOFPS" + to_string(frame), vector<string>{"VER"}, vector<string>{ver + " [" + verStage + "]"});
                    else hncip.script("logo.chns", "LOGOFPS" + to_string(frame), vector<string>{"VER"}, vector<string>{ver});
                    this_thread::sleep_for(chrono::milliseconds(1000 / fps));
                    frame++;
                }
                running = false;
            }).detach();
        } else {
            if (verStage != "Release") hncip.script("logo.chns", "LOGO", vector<string>{"VER"}, vector<string>{ver + " [" + verStage + "]"});
            else hncip.script("logo.chns", "LOGO", vector<string>{"VER"}, vector<string>{ver});
        }
        mi.mouse.btnAdd("PLAY", 2, 8, 30, 3);
        mi.mouse.btnAdd("SETTINGS", 2, 14, 30, 3);
        mi.mouse.btnAdd("QUIT", 2, 17, 30, 3);
        mi.mouse.cbCreate("HOME", [&](const string& btnName){
            if (btnName == "PLAY") chse = 1;
            if (btnName == "SETTINGS") chse = 3;
            if (btnName == "QUIT") chse = 4;
        });
        mi.async(3);
        if (cfg.settings.logo == 1) {
            logoAnimation = false;
            while(running);
        }
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