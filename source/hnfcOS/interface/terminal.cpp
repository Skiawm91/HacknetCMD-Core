#include "interface.h"
#include "../../input/input.h"
#include "../../console/console.h"
#include <sstream>
#include <vector>
using namespace std;

extern ManageInput mi;
extern Console con;

void Terminal() {
    mi.kbEnable();
    extern string playerIP;
    string targetIP;
    string fullCommand, cmd;
    vector<string> command;
    inputMasked = inputAte = false;
    kbPrompt = targetIP + "> ";
    mi.spReset();
    mi.async(11);
    if (enterDetected) {
        enterDetected = false;
        fullCommand = mi.getInput();
        if (!fullCommand.empty()) {
            istringstream iss(fullCommand);
            cmd.clear();
            command.clear();
            while (iss >> cmd) command.push_back(cmd);
            if (command[0] == "connect") {
                if (command[1] == playerIP) targetIP = playerIP + "@";
            }
            if(command[0] == "disconnect" || command[0] == "dc") targetIP.clear();
        }
        con.bufferSave(2); // 儲存終端機內容
    }
}