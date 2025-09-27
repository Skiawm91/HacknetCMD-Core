#define _HAS_STD_BYTE 0
#include "os.h"
#include "../function/function.h"
#include "../console/console.h"
#include "../config/config.h"
#include "../input/input.h"
#include "../hnasm/hnasm.h"
#include "interface/interface.h"
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

void hnfcOS::Interface() {
    func.audio.play("Revolve.wav");
    extern string playerName;
    extern string playerLang;
    extern string playerIP;
    int mode = 0;
    mi.btnAdd("EXIT", 0, 0, 3, 1);
    mi.btnAdd("TERMINAL", 5, 0, 10, 1);
    mi.btnAdd("DISPLAY", 16, 0, 9, 1);
    mi.cbCreate([&](const string& btnName){
        if (btnName == "EXIT");
        if (btnName == "TERMINAL") mode = 0;
        if (btnName == "DISPLAY") mode = 1;
    });
    while(true) {
        con.printAt(0, 0, string("|X|//|TERMINAL|/|DISPLAY|") + string(120 * (cfg.settings.cmdsize + 1) - 25, '/'));
        con.printAt(0, 1, string(120 * (cfg.settings.cmdsize + 1), '‾'));
        if (mode == 0) con.bufferRestore();
        while(mode == 0) Terminal();
        while(mode == 1) Display();
        con.clear();
    }
}