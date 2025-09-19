#define _HAS_STD_BYTE 0
#include "UI.h"
#include "function/function.h"
#include "config/config.h"
#include "../input/input.h"
#include "../hnasm/hnasm.h"
#include <filesystem>
#include <fstream>
using namespace std;

extern ManageInput mi;
extern Function func;
extern Config cfg;
extern UserInterface UI;

void UserInterface::Home(){
    int chse;
    func.audio.stop();
    func.audio.play("AmbientDroneClipped.wav");
    while(true) {
        mi.kbDisable();
        chse = 0;
        HNASM("ui.chns", "LOGO");
        HNASM("ui.chns", "HOME");
        mi.btnAdd("PLAY", 2, 8, 20, 3);
        mi.btnAdd("SETTINGS", 2, 14, 20, 3);
        mi.btnAdd("QUIT", 2, 17, 20, 3);
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
                while(true) {
                    func.cmd.clear();
                    mi.kbEnable();
                    HNASM("settings.chns", "VERBOSE_" + to_string(cfg.settings.verbose));
                    mi.btnAdd("VERBOSE", 0, 0, 20, 3);
                    mi.cbCreate([](const string& btnName){
                        if (btnName == "VERBOSE") {
                            cfg.settings.verbose = !cfg.settings.verbose;
                            if (!filesystem::exists("config")) {
                                filesystem::create_directory("config");
                            }
                            ofstream cfgFile("config/config.hund");
                            if (cfgFile.is_open()) {
                                cfgFile << "VERBOSE=" << cfg.settings.verbose << endl;
                                cfgFile.close();
                            }
                        }
                    });
                    mi.async(1);
                    if (enterDetected) {
                        enterDetected = false;
                    } else if (escDetected) {
                        escDetected = false;
                        mi.btnDel(vector<string>{"VERBOSE"});
                        mi.cbClean();
                        break;
                    }
                    mi.btnDel(vector<string>{"VERBOSE"});
                    mi.cbClean();
                }
                break;
            case 4:
                {
                    while(true) {
                        chse = 0;
                        HNASM("ui.chns", "QUIT");
                        mi.btnAdd("QUIT", 1, 2, 20, 3);
                        mi.btnAdd("CANCEL", 1, 5, 20, 3);
                        mi.cbCreate([&](const string& btnName){
                            if (btnName == "QUIT") {
                                chse = 1;
                            }
                            if (btnName == "CANCEL") {
                                chse = 2;
                            }
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