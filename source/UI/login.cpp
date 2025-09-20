#define _HAS_STD_BYTE 0
#include "UI.h"
#include "../input/input.h"
#include "../config/config.h"
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
using namespace std;

string playerName;
extern ManageInput mi;
extern Config cfg;
extern hnfcOS os;

void UserInterface::Login() {
    string input, shapwd;
    string name, tgshapwd;
    int chse;
    while(true) {
        mi.kbEnable();
        chse = 0;
        HNASM("ui.chns", "LOGO");
        HNASM("ui.chns", "USER");
        mi.btnAdd("LOGIN", 2, 8, 30, 3);
        mi.btnAdd("REGISTER", 2, 11, 30, 3);  
        mi.btnAdd("GUEST", 2, 14, 30, 3);
        mi.btnAdd("BACK", 2, 17, 30, 3);
        mi.cbCreate([&](const string& btnName) {
            if (btnName == "LOGIN") chse = 1;
            if (btnName == "REGISTER") chse = 2;
            if (btnName == "GUEST") chse = 3;
            if (btnName == "BACK") chse = 4;
        });
        mi.async(1);
        if (escDetected) {
            escDetected = false;
            chse = 4;
        } else if (enterDetected) {
            enterDetected = false;
        }
        mi.btnDel(vector<string>{"LOGIN", "REGISTER", "GUEST", "BACK"});
        mi.cbClean();
        switch(chse) {
            case 1:
                while(true) {
                    HNASM("logUI/login.chns", "NAME");
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
                        tgshapwd = cfg.data.load("config/" + name + "/pw.hnud", 0);
                        if (!cfg.data.loaded) HNASM("logUI/login.chns", "ERROR");
                        else {
                            HNASM("logUI/login.chns", "PASSWD");
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
                                playerName = cfg.data.load("config/" + name + "/name.hund", 0);
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
                    string pwd[2];
                    while (true) {
                        HNASM("logUI/register.chns", "REGISTER");
                        HNASM("logUI/register.chns", "NAME");
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
                        HNASM("logUI/register.chns", "PASSWD");
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
                        HNASM("logUI/register.chns", "CONFIRM");
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
                        HNASM("logUI/register.chns", "DETAILS");
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
                            if (chse == 2) {
                                break;
                            } else {
                                string nameOrigin = name;
                                transform(name.begin(), name.end(), name.begin(), ::tolower);
                                try {
                                    filesystem::create_directory("config/" + name);
                                } catch(...) {
                                    HNASM("logUI/register.chns", "RESERVED");
                                    break;
                                }
                                cfg.data.save("config/" + name + "/name.hund", nameOrigin);
                                if (pwd[0] == pwd[1]) {
                                    shapwd = SHA256Encrypt(pwd[1]);
                                    cfg.data.save("config/" + name + "/pw.hnud", shapwd);
                                    return;
                                } else {
                                    HNASM("logUI/register.chns", "INVCONFIRM");
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            case 3:
                playerName = "Guest";
                os.Boot();
                return;
            case 4:
                return;
            default:
                break;
        }
    }
}