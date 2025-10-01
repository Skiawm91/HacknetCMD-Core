#define _HAS_STD_BYTE 0
#include "../input/input.h"
#include "../config/config.h"
#include "../misc/misc.h"
#include "../hnasm/hnasm.h"
#include "../crypto/crypto.h"
#include "../hnfcOS/os.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>
module UI;
using namespace std;

extern ManageInput mi;
extern Misc misc;
extern Config cfg;
extern hnfcOS os;
extern string ver, verStage;
string playerName, playerLang;

void UserInterface::Login() {
    string input, shapwd;
    string name, tgshapwd;
    playerName = cfg.data.load("config/booted.hnd", 0);
    int chse;
    while(true) {
        mi.kbEnable();
        chse = 0;
        if (verStage != "Release") HNASM("ui.chns", "LOGO", "VER", ver + " [" + verStage + "]");
        else HNASM("ui.chns", "LOGO", "VER", ver);
        if (!playerName.empty()) HNASM("ui.chns", "USER", "PLAYER", string(playerName + "]") + string(16 - playerName.size(), ' '));
        else HNASM("ui.chns", "USER", "PLAYER", string("N/A]") + string(13, ' '));
        mi.btnAdd("LOGIN", 2, 8, 30, 3);
        if (!playerName.empty()) mi.btnAdd("CONTINUE", 2, 11, 30, 3);  
        mi.btnAdd("REGISTER", 2, 14, 30, 3);  
        mi.btnAdd("BACK", 2, 17, 30, 3);
        mi.cbCreate([&](const string& btnName) {
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
        mi.btnDel(vector<string>{"LOGIN", "CONTINUE", "REGISTER", "BACK"});
        mi.cbClean();
        switch(chse) {
            case 1:
                while(true) {
                    HNASM("logUI/login.chns", "NAME_" + misc.toLangName(cfg.settings.language));
                    mi.spReset();
                    mi.async(2);
                    if (escDetected) {
                        escDetected = false;
                        break;
                    } else if (enterDetected) {
                        enterDetected = false;
                        input = mi.getInput();
                    }
                    transform(input.begin(), input.end(), input.begin(), ::tolower);
                    name = input;
                    {
                        tgshapwd = cfg.data.load("config/" + name + "/pw.hnd", 0);
                        if (!cfg.data.loaded) HNASM("logUI/login.chns", "ERROR");
                        else {
                            HNASM("logUI/login.chns", "PASSWD_" + misc.toLangName(cfg.settings.language));
                            mi.spReset();
                            mi.async(2);
                            if (escDetected) {
                                escDetected = false;
                                break;
                            } else if (enterDetected) {
                                enterDetected = false;
                                input = mi.getInput();
                            }
                            shapwd = SHA256Encrypt(input);
                            if (shapwd == tgshapwd) {
                                playerName = cfg.data.load("config/" + name + "/info.hnd", 0);
                                playerLang = cfg.data.load("config/" + name + "/info.hnd", 1);
                                cfg.data.del("config/booted.hnd", playerName);
                                os.Boot();
                                return;
                            } else {
                                HNASM("logUI/login.chns", "ERROR");
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
                    return;
                }
                break;
            case 3:
                {
                    string pwd[2];
                    while (true) {
                        HNASM("logUI/register.chns", "REGISTER");
                        HNASM("logUI/register.chns", "NAME_" + misc.toLangName(cfg.settings.language));
                        mi.spReset();
                        mi.async(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.getInput();
                        }
                        name = input;
                        HNASM("logUI/register.chns", "PASSWD_" + misc.toLangName(cfg.settings.language));
                        mi.spReset();
                        mi.async(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.getInput();
                        }
                        pwd[0] = input;
                        HNASM("logUI/register.chns", "CONFIRM_" + misc.toLangName(cfg.settings.language));
                        mi.spReset();
                        mi.async(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.getInput();
                        }
                        pwd[1] = input;
                        chse = 0;
                        HNASM("logUI/register.chns", "DETAILS_" + misc.toLangName(cfg.settings.language));
                        while(true) {
                            mi.btnAdd("CONFIRM", 1, 8, 20, 3);
                            mi.btnAdd("CANCEL", 1, 11, 20, 3);
                            mi.cbCreate([&](const string& btnName){
                                if (btnName == "CANCEL") chse = 2;
                            });
                            mouseSync = true;
                            mi.spReset();
                            mi.async(1);
                            if (escDetected) {
                                escDetected = false;
                                chse = 2;
                            } else if (enterDetected) {
                                enterDetected = false;
                            }
                            mi.btnDel(vector<string>{"CONFIRM", "CANCEL"});
                            mi.cbClean();
                            if (chse == 2) break;
                            else {
                                if (name.length() > 13) {
                                    HNASM("logUI/register.chns", "TOOLONG");
                                }
                                string nameOrigin = name;
                                transform(name.begin(), name.end(), name.begin(), ::tolower);
                                try {
                                    filesystem::create_directory("config/" + name);
                                } catch(...) {
                                    HNASM("logUI/register.chns", "RESERVED");
                                    break;
                                }
                                cfg.data.save("config/" + name + "/info.hnd", nameOrigin);
                                if (pwd[0] == pwd[1]) {
                                    shapwd = SHA256Encrypt(pwd[1]);
                                    cfg.data.save("config/" + name + "/pw.hnd", shapwd);
                                } else {
                                    HNASM("logUI/register.chns", "INVCONFIRM");
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