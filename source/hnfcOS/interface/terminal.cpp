#include "../os.h"
#include "../../input/input.h"
#include "../../config/config.h"
#include "../../console/console.h"
#include <sstream>
#include <vector>
using namespace std;

extern ManageInput mi;
extern Config cfg;
extern Console con;
extern string playerIP;

void hnfcOS::Terminal() {
    mi.kbEnable();
    #ifdef __APPLE__
    extern int promptPos;
    #endif
    static string targetIP;
    string fullCommand, cmd;
    vector<string> command;
    inputMasked = inputAte = false;
    if (!targetIP.empty()) kbPrompt = targetIP + "@> ";
    else kbPrompt = "> ";
    mi.spReset();
    mi.async(11);
    if (enterDetected) {
        enterDetected = false;
        fullCommand = mi.getInput();
        if (!fullCommand.empty()) {
            istringstream iss(fullCommand);
            while (iss >> cmd) command.push_back(cmd);
            if (command[0] == "connect") {
                if (command[1] == playerIP) targetIP = playerIP;
                else targetIP = playerIP;
            }
            if(command[0] == "disconnect" || command[0] == "dc") targetIP.clear();
        }
        #ifdef _WIN32
        con.bufferSave(2); // 儲存終端機內容
        #endif
    }
}