#include "os.h"
#include "input.h"
#include "config.h"
#include "console.h"
#include "hnasm.h"
#include <sstream>
#include <vector>
#include <filesystem>
using namespace std;

extern ManageInput mi;
extern Config cfg;
extern Console con;
extern HNASM hnasm;
extern string playerIP;

void hnfcOS::Terminal() {
    mi.kb.enable();
    string fullCommand, cmd;
    vector<string> command;
    inputMasked = inputAte = false;
    if (!targetIP.empty()) kbPrompt = targetIP + "@> ";
    else kbPrompt = "> ";
    mi.kb.spReset();
    mi.async(11);
    if (enterDetected) {
        enterDetected = false;
        fullCommand = mi.kb.getInput();
        if (!fullCommand.empty()) {
            istringstream iss(fullCommand);
            while (iss >> cmd) command.push_back(cmd);
            if (command[0] == "connect") {
                if (command[1] == playerIP || command[1].empty()) targetIP = playerIP;
                else if (command[1] == "192.168.0.11") targetIP = "192.168.0.11";
                else if (command[1] == "192.168.0.12") targetIP = "192.168.0.12";
                else if (command[1] == "192.168.0.13") targetIP = "192.168.0.13";
                else if (command[1] == "192.168.0.14") targetIP = "192.168.0.14";
                else if (command[1] == "192.168.0.15") targetIP = "192.168.0.15";
                else {
                    vector<string> files;
                    for (const auto& entry : filesystem::directory_iterator("assets/nodes")) {
                        if (entry.is_regular_file() && entry.path().extension() == ".hnn") {
                            files.push_back(entry.path().filename().string());
                        }
                    }
                    for (const auto &f : files) {
                        auto IP = get<0>(hnasm.node(f));
                        if (command[1] == IP) targetIP = IP;
                    }
                }
            }
            if(command[0] == "disconnect" || command[0] == "dc") targetIP.clear();
        }
        #ifdef _WIN32
        con.bufferSave(2); // 儲存終端機內容
        #endif
    }
}