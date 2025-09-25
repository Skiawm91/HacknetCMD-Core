#define _HAS_STD_BYTE 0
#include "os.h"
#include "../function/function.h"
#include "../misc/misc.h"
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
extern Misc misc;
extern ManageInput mi;
// HacknetStory hnStory;

void hnfcOS::Interface() {
    func.audio.play("Revolve.wav");
    extern string playerName;
    extern string playerLang;
    extern string playerIP;
    string lowerName;
    lowerName.resize(playerName.size());
    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
    string line;
    ifstream saveFile("config/" + lowerName + "/save.hnd");
    if (saveFile.is_open()) {
        while (getline(saveFile, line)) {
            if (line == "434F4E4649472E5455544F5249414C3A46414C5345") {
                HNASM("terminal/initial.chns", "INITIAL");
                HNASM("terminal/initial.chns", "HELPMSG");
                break;
            } else {
                // HNASM("tutorial/failsafe.chns", "FAILSAFE_" + playerLang);
                HNASM("terminal/initial.chns", "INITIAL");
                // HNASM("terminal/initial.chns", "TUTORIAL");
                // s.Tutorial();
                break;
            }
        }
    }
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
        // misc.staticPrint(0, 0, "|X|//|TERMINAL|/|DISPLAY|" + string(120 * cfg.settings.cmdsize - 25, '/'));
        if (mode == 0) Terminal();
        else if (mode == 1) Display();
    }
}