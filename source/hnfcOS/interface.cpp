#define _HAS_STD_BYTE 0
#include "os.h"
#include "function.h"
#include "console.h"
#include "config.h"
#include "input.h"
#include "hnasm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

extern Function func;
extern Config cfg;
extern Console con;
extern ManageInput mi;
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

void hnfcOS::Interface() {
    func.audio.stop();
    func.audio.play("Revolve.wav");
    extern string playerName;
    extern string playerLang;
    extern string playerIP;
    targetIP.clear();
    #ifdef __APPLE__
    extern int promptPos;
    #endif
    int mode = 1; // Terminal
    mi.btnAdd("EXIT", 0, 0, 3, 1);
    mi.btnAdd("TERMINAL", 5, 0, 10, 1);
    mi.btnAdd("DISPLAY", 16, 0, 9, 1);
    mi.cbCreate([&](const string& btnName){
        if (btnName == "EXIT") mode = 0;
        if (btnName == "TERMINAL") mode = 1;
        if (btnName == "DISPLAY") mode = 2;
    });
    while(true) {
        #ifdef __APPLE__
        if (mode == 1) con.bufferChange(0);
        else if (mode == 2) con.bufferChange(1);
        #endif
        con.printAt(0, 0, string("|X|//|TERMINAL|/|DISPLAY|") + string(120 * (cfg.settings.cmdsize + 1) - 25, '/'));
        con.printAt(0, 1, overlines(120 * (cfg.settings.cmdsize + 1)));
        if (mode == 0) {
            mi.btnDel(vector<string>{"EXIT", "TERMINAL", "DISPLAY"});
            mi.cbClean();
            return;
        } else if (mode == 1) {
            #ifdef _WIN32
            con.bufferRestore();
            #elif __APPLE__
            cout << "\033[2K\r" << flush; // 先清掉
            #endif
            Terminal();
        } else if (mode == 2) Display();
        #ifdef _WIN32
        con.clear();
        #endif
    }
}