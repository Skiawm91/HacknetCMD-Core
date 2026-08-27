#define _HAS_STD_BYTE 0
#include "UI.h"
#include "crypto.h"
#include "function.h"
#include "data.h"
#include "input.h"
#include "HNCIP.h"
#include "misc.h"
#include "console.h"
#include "os.h"
#include "discord-rpc/discord_rpc.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
using namespace std;

extern ManageInput mi;
extern Function func;
extern Data dta;
extern UserInterface UI;
extern Misc misc;
extern hnfcOS os;
extern string ver, verStage;
extern HNCInterPreter hncip;
extern DiscordRichPresence drp;
extern Console con;
string playerName, playerLang;

void UserInterface::Home(){
    string input, shapwd;
    string name, tgshapwd;
    playerName = dta.load("data/booted.hnd", 0);
    int chse;
    atomic<bool> running, logoAnimation;
    func.audio.stop();
    func.audio.playL("ADC", vector<string>{"AmbientDrone_Clipped.ogg"});
    while(true) {
        con.clear();
        con.setColorBg("000000");
        con.cursor.hide();
        drp.state = "Home";
        Discord_UpdatePresence(&drp);
        mi.kb.disable();
        chse = 0;
        if (!playerName.empty()) hncip.script("ui.chns", "HOME_" + misc.toLangName(dta.cfg.language), vector<string>{"PLAYER"}, vector<string>{string(playerName + "]") + string(35 - playerName.size(), ' ')});
        else hncip.script("ui.chns", "HOME_" + misc.toLangName(dta.cfg.language), vector<string>{"PLAYER"}, vector<string>{string("N/A]") + string(32, ' ')});
        if (dta.cfg.dynamicLogo == 1) {
            logoAnimation = true;
            thread([&]{
                running = true;
                int fps = 60, frame = 1;
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
        mi.mouse.btnAdd("NEWSESSION", 2, 8, 30, 3);
        mi.mouse.btnAdd("CONTINUE", 2, 11, 30, 3);
        mi.mouse.btnAdd("LOGIN", 2, 14, 30, 3);
        // mi.mouse.btnAdd("EXTENSIONS", 2, 17, 30, 3); // 還沒做
        mi.mouse.btnAdd("SETTINGS", 2, 20, 30, 3);
        mi.mouse.btnAdd("QUIT", 2, 23, 30, 3);
        mi.mouse.cbCreate("HOME", [&](const string& btnName){
            if (btnName == "NEWSESSION") chse = 1;
            if (btnName == "CONTINUE") chse = 2;
            if (btnName == "LOGIN") chse = 3;
            if (btnName == "EXTENSIONS") chse = 4;
            if (btnName == "SETTINGS") chse = 5;
            if (btnName == "QUIT") chse = 6;
        });
        mi.sync(3);
        if (dta.cfg.dynamicLogo == 1) {
            logoAnimation = false;
            while(running);
        }
        mi.mouse.btnDel(vector<string>{"NEWSESSION", "CONTINUE", "LOGIN", "EXTENSIONS", "SETTINGS", "QUIT"});
        mi.mouse.cbClean();
        switch(chse) {
            case 1:
                {
                    string pwd[2];
                    while (true) {
                        con.cursor.show();
                        mi.kb.enable();
                        hncip.script("logUI/register.chns", "REGISTER");
                        hncip.script("logUI/register.chns", "NAME_" + misc.toLangName(dta.cfg.language));
                        mi.kb.historyClear();
                        mi.kb.spReset();
                        mi.sync(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.kb.getInput();
                        }
                        name = input;
                        hncip.script("logUI/register.chns", "PASSWD_" + misc.toLangName(dta.cfg.language));
                        mi.kb.historyClear();
                        mi.kb.spReset();
                        mi.sync(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.kb.getInput();
                        }
                        pwd[0] = input;
                        hncip.script("logUI/register.chns", "CONFIRM_" + misc.toLangName(dta.cfg.language));
                        mi.kb.historyClear();
                        mi.kb.spReset();
                        mi.sync(2);
                        if (escDetected) {
                            escDetected = false;
                            break;
                        } else if (enterDetected) {
                            enterDetected = false;
                            input = mi.kb.getInput();
                        }
                        pwd[1] = input;
                        chse = 0;
                        con.cursor.hide();
                        hncip.script("logUI/register.chns", "DETAILS_" + misc.toLangName(dta.cfg.language));
                        while(true) {
                            mi.mouse.btnAdd("CONFIRM", 1, 8, 20, 3);
                            mi.mouse.btnAdd("CANCEL", 1, 11, 20, 3);
                            mi.mouse.cbCreate("DETAILS", [&](const string& btnName){
                                if (btnName == "CANCEL") chse = 2;
                            });
                            mouseSync = true;
                            mi.kb.spReset();
                            mi.sync(1);
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
                                    hncip.script("logUI/register.chns", "TOOLONG");
                                }
                                string nameOrigin = name;
                                transform(name.begin(), name.end(), name.begin(), ::tolower);
                                try {
                                    filesystem::create_directory("data/" + name);
                                } catch(...) {
                                    hncip.script("logUI/register.chns", "RESERVED");
                                    break;
                                }
                                dta.save("data/" + name + "/info.hnd", nameOrigin);
                                if (pwd[0] == pwd[1]) {
                                    shapwd = SHA256Encrypt(pwd[1]);
                                    dta.save("data/" + name + "/pw.hnd", shapwd);
                                } else {
                                    hncip.script("logUI/register.chns", "INVCONFIRM");
                                    break;
                                }
                                dta.save("data/" + name + "/info.hnd", misc.toLangName(dta.cfg.language));
                                // Register ended
                                playerName = dta.load("data/" + name + "/info.hnd", 0);
                                playerLang = dta.load("data/" + name + "/info.hnd", 1);
                                dta.del("data/booted.hnd", playerName);
                                os.Boot();
                                func.audio.stop();
                                func.audio.playL("ADC", vector<string>{"AmbientDrone_Clipped.ogg"});
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
                    playerLang = dta.load("data/" + lowerName + "/info.hnd", 1);
                    os.Initial(false);
                    func.audio.stop();
                    func.audio.playL("ADC", vector<string>{"AmbientDrone_Clipped.ogg"});
                }
            break;
            case 3:
                while(true) {
                    con.cursor.show();
                    mi.kb.enable();
                    hncip.script("logUI/login.chns", "NAME_" + misc.toLangName(dta.cfg.language));
                    mi.kb.historyClear();
                    mi.kb.spReset();
                    mi.sync(2);
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
                        tgshapwd = dta.load("data/" + name + "/pw.hnd", 0);
                        if (!dta.loaded) hncip.script("logUI/login.chns", "ERROR");
                        else {
                            hncip.script("logUI/login.chns", "PASSWD_" + misc.toLangName(dta.cfg.language));
                            mi.kb.historyClear();
                            mi.kb.spReset();
                            mi.sync(2);
                            if (escDetected) {
                                escDetected = false;
                                break;
                            } else if (enterDetected) {
                                enterDetected = false;
                                input = mi.kb.getInput();
                            }
                            shapwd = SHA256Encrypt(input);
                            if (shapwd == tgshapwd) {
                                playerName = dta.load("data/" + name + "/info.hnd", 0);
                                playerLang = dta.load("data/" + name + "/info.hnd", 1);
                                dta.del("data/booted.hnd", playerName);
                                os.Boot();
                                func.audio.stop();
                                func.audio.playL("ADC", vector<string>{"AmbientDrone_Clipped.ogg"});
                            } else {
                                hncip.script("logUI/login.chns", "ERROR");
                            }
                        }
                    }
                }
                break;
            case 4:
                {
                    // 什麼都沒有
                }
                break;
            case 5:
                UI.Settings();
                break;
            case 6:
                {
                    while(true) {
                        chse = 0;
                        hncip.script("ui.chns", "QUIT_" + misc.toLangName(dta.cfg.language));
                        mi.mouse.btnAdd("QUIT", 1, 2, 30, 3);
                        mi.mouse.btnAdd("CANCEL", 1, 5, 30, 3);
                        mi.mouse.cbCreate("QUIT", [&](const string& btnName){
                            if (btnName == "QUIT") chse = 1;
                            if (btnName == "CANCEL") chse = 2;
                        });
                        mi.sync(3);
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