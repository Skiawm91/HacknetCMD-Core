#define _HAS_STD_BYTE 0
#include "os.h"
#include "UI.h"
#include "function.h"
#include "console.h"
#include "data.h"
#include "input.h"
#include "HNCIP.h"
#include "discord_rpc.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

extern UserInterface UI;
extern Function func;
extern Data dta;
extern Console con;
extern ManageInput mi;
extern DiscordRichPresence drp;
// HacknetStory hnStory;

string overlines(int count) {
    const char* ul = reinterpret_cast<const char*>(u8"‾");
    std::string result;
    result.reserve(count * 3);
    for (int i = 0; i < count; ++i) {
        result += ul;
    }
    return result;
}

void menuBarCb(int& backupMode, int& mode) {
    mi.mouse.cbCreate("MENUBAR", [&](const string& btnName){
        if (btnName == "EXIT") mode = 0;
        if (btnName == "SETTINGS") {
            backupMode = mode;
            mode = 1;
        }
        // if (btnName == "SAVE") sessionSave();
        if (btnName == "TERMINAL") mode = 2;
        if (btnName == "DISPLAY") mode = 3;
        if (btnName == "NETMAP") mode = 4;
        if (btnName == "RAM") mode = 5;
        if (btnName == "MAIL") mode = 6;
    });
}

void hnfcOS::MenuBar() {
    #ifdef _WIN32
    int preRemove = 53;
    #elif __APPLE__
    int preRemove = 51;
    #endif
    con.printAt(0, 0, string("|❌||⚙️||💾|//|TERMINAL|/|DISPLAY|/|NETMAP|/|RAM|") + string(144 + 12 * dta.cfg.cmdsize - preRemove, '/') + string("|✉️|"));
    con.printAt(0, 1, overlines(144 + 12 * dta.cfg.cmdsize));
}

void hnfcOS::Interface() {
    func.audio.stop();
    func.audio.playL("InGame", vector<string>{"Revolve.wav", "out_run_the_wolves.wav"});
    string state;
    extern string playerName;
    extern string playerLang;
    extern string playerIP;
    int mode = 2; // Terminal
    int backupMode;
    mi.mouse.btnAdd("EXIT", 0, 0, 4, 1);
    mi.mouse.btnAdd("SETTINGS", 4, 0, 4, 1);
    mi.mouse.btnAdd("SAVE", 8, 0, 4, 1);
    mi.mouse.btnAdd("TERMINAL", 14, 0, 10, 1);
    mi.mouse.btnAdd("DISPLAY", 25, 0, 9, 1);
    mi.mouse.btnAdd("NETMAP", 35, 0, 8, 1);
    mi.mouse.btnAdd("RAM", 44, 0, 5, 1);
    mi.mouse.btnAdd("MAIL", 144 * (dta.cfg.cmdsize + 1) - 4, 0, 4, 1);
    menuBarCb(backupMode, mode);
    while(true) {
        if (!targetIP.empty()) state = "Location: " + targetIP;
        else state = "Location: Not Connected";
        drp.state = state.c_str();
        Discord_UpdatePresence(&drp);
        #ifdef __APPLE__
        if (mode == 2) con.bufferChange(0);
        else if (mode == 1 || mode == 3) con.bufferChange(1);
        #endif
        MenuBar();
        if (mode == 0) {
            sys.cleanNode();
            #ifdef __APPLE__
            con.bufferChange(0);
            #endif
            termTasks.clear();
            mi.mouse.btnDel(vector<string>{"EXIT", "SETTINGS", "SAVE", "TERMINAL", "DISPLAY", "NETMAP", "RAM", "MAIL"});
            mi.mouse.cbClean();
            return;
        } else if (mode == 1) {
            mi.mouse.cbClean("MENUBAR");
            UI.Settings(true);
            menuBarCb(backupMode, mode);
            mode = backupMode;
        } else if (mode == 2) {
            #ifdef _WIN32
            con.bufferRestore();
            #elif __APPLE__
            cout << "\033[2K\r" << flush; // 先清掉
            #endif
            Terminal();
        } else if (mode == 3) Display();
        else if (mode == 4) NetMap();
        else if (mode == 5) RAMUI();
        else if (mode == 6) MailUI();
        #ifdef _WIN32
        con.clear();
        #endif
    }
}