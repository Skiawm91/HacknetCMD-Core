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

void Cmd() {
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
            if (line == "434F4E4649472E5455544F5249414C3A46414C5345") {
                HNASM("terminal/initial.chns", "HELPMSG");
                break;
            } else {
                HNASM("terminal/initial.chns", "TUTORIAL");
                break;
            }
        }
    }
    while (true) {
        cout << targetIP << "> ";
        inputMasked = false;
        mi.async(2);
        if (enterDetected) {
            enterDetected = false;
            fullCommand = mi.getInput();
        } else if (escDetected) {
            escDetected = false;
        }
    }
    istringstream iss(fullCommand);
    string cmd;
    cmd.clear();
    while (iss >> cmd) {
        command.push_back(cmd);
    }
    if (command[0] == "connect") {
        if (command[1] == ipAddress) targetIP = ipAddress + "@";
    }
}