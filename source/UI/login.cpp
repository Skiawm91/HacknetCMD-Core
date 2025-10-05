#define _HAS_STD_BYTE 0
#include "UI.h"
#include "crypto.h"
#include "config.h"
#include "console.h"
#include "misc.h"
#include "function.h"
#include "input.h"
#include "hnasm.h"
#include "os.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>
using namespace std;

extern ManageInput mi;
extern Misc misc;
extern Config cfg;
extern Function func;
extern hnfcOS os;
extern HNASM hnasm;
extern string ver, verStage;
string playerName, playerLang;

void UserInterface::Login() {
    string input, shapwd;
    string name, tgshapwd;
    playerName = cfg.data.load("config/booted.hnd", 0);
    int chse;
    while(true) {
        mi.kb.enable();
        chse = 0;
        if (verStage != "Release") hnasm.script("ui.chns", "LOGO", vector<string>{"VER"}, vector<string>{ver + " [" + verStage + "]"});
        else hnasm.script("ui.chns", "LOGO", vector<string>{"VER"}, vector<string>{ver});
        if (!playerName.empty()) hnasm.script("ui.chns", "USER", vector<string>{"PLAYER"}, vector<string>{string(playerName + "]") + string(16 - playerName.size(), ' ')});
        else hnasm.script("ui.chns", "USER", vector<string>{"PLAYER"}, vector<string>{string("N/A]") + string(13, ' ')});
        mi.mouse.btnAdd("LOGIN", 2, 8, 30, 3);
        if (!playerName.empty()) mi.mouse.btnAdd("CONTINUE", 2, 11, 30, 3);  
        mi.mouse.btnAdd("REGISTER", 2, 14, 30, 3);  
        mi.mouse.btnAdd("BACK", 2, 17, 30, 3);
        mi.mouse.cbCreate("USER", [&](const string& btnName) {
            if (btnName == "LOGIN") chse = 1;
            if (btnName == "CONTINUE") chse = 2;
            if (btnName == "REGISTER") chse = 3;
            if (btnName == "BACK") chse = 4;
        });
        mi.async(1);
        if (escDetected) {
            escDetected = false;
            chse = 4;
        } else if (enterDetected) {
            enterDetected = false;
        }
        mi.mouse.btnDel(vector<string>{"LOGIN", "CONTINUE", "REGISTER", "BACK"});
        mi.mouse.cbClean();
        switch(chse) {
            case 1:
                while(true) {
                    hnasm.script("logUI/login.chns", "NAME_" + misc.toLangName(cfg.settings.language));
                    mi.kb.spReset();
                    mi.async(2);
                    if (escDetected) {
                        escDetected = false;
                        break;
                    } else if (enterDetected) {
                        enterDetected = false;
                        input = mi.kb.getInput();
                    }
                    transform(input.begin(), input.end(), input.begin(), ::tolower);
                    name = input;
                    {
                        tgshapwd = cfg.data.load("config/" + name + "/pw.hnd", 0);
                        if (!cfg.data.loaded) hnasm.script("logUI/login.chns", "ERROR");
                        else {
                            hnasm.script("logUI/login.chns", "PASSWD_" + misc.toLangName(cfg.settings.language));
                            mi.kb.spReset();
                            mi.async(2);
                            if (escDetected) {
                                escDetected = false;
                                break;
                            } else if (enterDetected) {
                                enterDetected = false;
                                input = mi.kb.getInput();
                            }
                            shapwd = SHA256Encrypt(input);
                            if (shapwd == tgshapwd) {
                                playerName = cfg.data.load("config/" + name + "/info.hnd", 0);
                                playerLang = cfg.data.load("config/" + name + "/info.hnd", 1);
                                cfg.data.del("config/booted.hnd", playerName);
                                os.Boot();
                                func.audio.stop();
                                func.audio.play("AmbientDroneClipped.wav");
                                return;
                            } else {
                                hnasm.script("logUI/login.chns", "ERROR");
                            }
                        }
                    }
                }
                break;
            case 2:
                {
                    string lowerName;
                    lowerName.resize(playerName.size());
                    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
                    playerLang = cfg.data.load("config/" + lowerName + "/info.hnd", 1);
                    os.Initial(false);
                    func.audio.stop();
                    func.audio.play("AmbientDroneClipped.wav");
                    return;
                }
                break;
            case 3:
                {
                    string pwd[2];
                    while (true) {
                        hnasm.script("logUI/register.chns", "REGISTER");
                        hnasm.script("logUI/register.chns", "NAME_" + misc.toLangName(cfg.settings.language));
                        mi.kb.spReset();
                        mi.async(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.kb.getInput();
                        }
                        name = input;
                        hnasm.script("logUI/register.chns", "PASSWD_" + misc.toLangName(cfg.settings.language));
                        mi.kb.spReset();
                        mi.async(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.kb.getInput();
                        }
                        pwd[0] = input;
                        hnasm.script("logUI/register.chns", "CONFIRM_" + misc.toLangName(cfg.settings.language));
                        mi.kb.spReset();
                        mi.async(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.kb.getInput();
                        }
                        pwd[1] = input;
                        chse = 0;
                        hnasm.script("logUI/register.chns", "DETAILS_" + misc.toLangName(cfg.settings.language));
                        while(true) {
                            mi.mouse.btnAdd("CONFIRM", 1, 8, 20, 3);
                            mi.mouse.btnAdd("CANCEL", 1, 11, 20, 3);
                            mi.mouse.cbCreate("DETAILS", [&](const string& btnName){
                                if (btnName == "CANCEL") chse = 2;
                            });
                            mouseSync = true;
                            mi.kb.spReset();
                            mi.async(1);
                            if (escDetected) {
                                escDetected = false;
                                chse = 2;
                            } else if (enterDetected) {
                                enterDetected = false;
                            }
                            mi.mouse.btnDel(vector<string>{"CONFIRM", "CANCEL"});
                            mi.mouse.cbClean();
                            if (chse == 2) break;
                            else {
                                if (name.length() > 13) {
                                    hnasm.script("logUI/register.chns", "TOOLONG");
                                }
                                string nameOrigin = name;
                                transform(name.begin(), name.end(), name.begin(), ::tolower);
                                try {
                                    filesystem::create_directory("config/" + name);
                                } catch(...) {
                                    hnasm.script("logUI/register.chns", "RESERVED");
                                    break;
                                }
                                cfg.data.save("config/" + name + "/info.hnd", nameOrigin);
                                if (pwd[0] == pwd[1]) {
                                    shapwd = SHA256Encrypt(pwd[1]);
                                    cfg.data.save("config/" + name + "/pw.hnd", shapwd);
                                } else {
                                    hnasm.script("logUI/register.chns", "INVCONFIRM");
                                    break;
                                }
                                cfg.data.save("config/" + name + "/info.hnd", misc.toLangName(cfg.settings.language));
                                return;
                            }
                        }
                    }
                }
                break;
            case 4:
                return;
            default:
                break;
        }
    }
}