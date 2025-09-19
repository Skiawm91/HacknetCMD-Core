#define _HAS_STD_BYTE 0
#include "UI.h"
#include "input/input.h"
#include "hnasm/hnasm.h"
#include "crypto/crypto.h"
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
extern hnfcOS os;

void UserInterface::Login() {
    string name;
    extern string input;
    extern string pwdtext;
    extern string shatext;
    int chse;
    string shapwd, tgshapwd;
    while(true) {
        mi.kbEnable();
        chse = 0;
        HNASM("ui.chns", "LOGO");
        HNASM("ui.chns", "USER");
        mi.btnAdd("LOGIN", 2, 8, 20, 3);
        mi.btnAdd("REGISTER", 2, 11, 20, 3);  
        mi.btnAdd("GUEST", 2, 14, 20, 3);
        mi.btnAdd("BACK", 2, 17, 20, 3);
        mi.cbCreate([&](const string& btnName) {
            if (btnName == "LOGIN") {
                chse = 1;
            }
            if (btnName == "REGISTER") {
                chse = 2;
            }
            if (btnName == "GUEST") {
                chse = 3;
            }
            if (btnName == "BACK") {
                chse = 4;
            }
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
                        ifstream file("config/" + name + "/pw.hnud");
                        if (!file) {HNASM("logUI/login.chns", "ERROR");}
                        else {
                            string line;
                            getline(file, line);
                            istringstream iss(line);
                            iss >> tgshapwd;
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
                            SHA256Encrypt(input);
                            shapwd = shatext;
                            if (shapwd == tgshapwd) {
                                ifstream file("config/" + name + "/name.hund");
                                if (file) {
                                    string line;
                                    getline(file, line);
                                    istringstream iss(line);
                                    iss >> playerName;
                                }
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
                                if (btnName == "CONFIRM") {
                                    // do nothing
                                }
                                if (btnName == "CANCEL") {
                                    chse = 2;
                                }
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
                                if (!filesystem::exists("config")) {
                                    filesystem::create_directory("config");
                                }
                                    string nameOrigin = name;
                                    transform(name.begin(), name.end(), name.begin(), ::tolower);
                                    try {
                                        filesystem::create_directory("config/" + name);
                                    } catch(...) {
                                        HNASM("logUI/register.chns", "RESERVED");
                                        break;
                                    }
                                    ofstream file("config/" + name + "/name.hund");
                                    if (file.is_open()) {
                                        file << nameOrigin << endl;
                                        file.close();
                                    }
                                    if (pwd[0] == pwd[1]) {
                                    SHA256Encrypt(pwd[1]);
                                    shapwd = shatext;
                                    ofstream file("config/" + name + "/pw.hnud");
                                    if (file.is_open()) {
                                        file << shapwd << endl;
                                        file.close();
                                    }
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