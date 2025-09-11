#define _HAS_STD_BYTE 0
#include "audio.h"
#include "input/input.h"
#include "hnasm/hnasm.h"
#include "clearScreen.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "cmd.h"
using namespace std;

// HacknetStory hs;

void Cmd() {
    StopAudio();
    PlayAudio("Revolve.wav");
    extern ManageInput mi;
    extern string playerName;
    string lowerName;
    lowerName.resize(playerName.size());
    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
    extern string ipAddress;
    cls();
    string targetIP;
    string fullCommand;
    vector<string> command;
    string line;
    HNASM("terminal/initial.chns", "INITIAL");
    ifstream saveFile("config/" + lowerName + "/save.hnd");
    if (saveFile.is_open()) {
        while (getline(saveFile, line)) {
            // if (line == "434F4E4649472E5455544F5249414C3A46414C5345") {
                HNASM("terminal/initial.chns", "HELPMSG");
                break;
            // } else {
                // HNASM("terminal/initial.chns", "TUTORIAL");
                // s.Tutorial();
                // break;
            // }
        }
    }
    while (true) {
        inputMasked = false;
        kbPrompt = targetIP + "> ";
        mi.spReset();
        mi.async(2);
        if (enterDetected) {
            enterDetected = false;
            fullCommand = mi.getInput();
        } else if (escDetected) {
            escDetected = false;
        }
        istringstream iss(fullCommand);
        string cmd;
        cmd.clear();
        command.clear();
        while (iss >> cmd) {
            command.push_back(cmd);
        }
        if (command[0] == "connect") {
            if (command[1] == ipAddress) targetIP = ipAddress + "@";
        }
        if(command[0] == "disconnect" || command[0] == "dc") targetIP.clear();
    }
}