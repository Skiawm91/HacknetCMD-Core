#define _HAS_STD_BYTE 0
#include "os.h"
#include "input.h"
#include "data.h"
#include "console.h"
#include "HNCIP.h"
#include <sstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <iostream>
#include <algorithm>
using namespace std;

extern ManageInput mi;
extern Data dta;
extern Console con;
extern HNCInterPreter hncip;
extern string playerIP;

void hnfcOS::Terminal() {
    Mode = "Terminal";
    con.cursor.show();
    mi.kb.enable();
    if (!termTasks.empty()) {
        for (const auto &tT : termTasks) tT();
        termTasks.clear();
        #ifdef _WIN32
        con.bufferSave(2); // 儲存終端機內容
        #endif
        return;
    }
    string fullCommand, cmd;
    vector<string> command;
    cmd.clear();
    command.clear();
    inputMasked = inputAte = false;
    if (!targetIP.empty()) {
        if (!path.empty()) kbPrompt = targetIP + path + "> ";
        else kbPrompt = targetIP + "@> ";
    } else kbPrompt = "> ";
    mi.kb.spReset();
    mi.sync(11);
    if (enterDetected) {
        enterDetected = false;
        fullCommand = mi.kb.getInput();
        if (!fullCommand.empty()) {
            istringstream iss(fullCommand);
            while (iss >> cmd) command.push_back(cmd);
            string lowerCmd;
            lowerCmd.resize(command[0].size());
            transform(command[0].begin(), command[0].end(), lowerCmd.begin(), ::tolower);
            if (command[0] == "connect") {
                string lastIP = targetIP;
                if (command.size() == 1 || command[1] == playerIP) targetIP = playerIP;
                else {
                    targetIP = command[1];
                    HNCInterPreter::NodeInfo tempNode = sys.getNode(targetIP);
                    if (tempNode.IP.empty()) targetIP.clear();
                }
                if (targetIP != lastIP) {
                    node = sys.getNode(targetIP);
                    path.clear();
                    displayChse = 0;
                }
            }
            else if (lowerCmd == "probe" || command[0] == "nmap") app.Probe(node);
            else if (command[0] == "cd") {
                if (command.size() >= 2) this->cmd.ChangeDir(node, command[1]);
                else std::cout << "Usage: cd [WHERE TO GO or .. TO GO BACK]" << std::endl;
            }
            else if (command[0] == "ls" || command[0] == "dir") this->cmd.ListDir(node);
            else if (command[0] == "cat") {
                if (command.size() >= 2) this->cmd.Concatenate(command[1], node);
                else std::cout << "Usage: cat [FILENAME]" << std::endl;
            }
            else if (lowerCmd == "rm") {
                if (command.size() >= 2) this->cmd.Remove(command[1], node);
                else std::cout << "Not Enough Arguments" << std::endl;
            }
            else if (lowerCmd == "mv") {
                if (command.size() >= 3) this->cmd.Move(command[1], command[2], node);
                else std::cout << "Not Enough Arguments. Usage: mv [FILE] [DESTINATION]" << std::endl;
            }
            else if (command[0] == "disconnect" || command[0] == "dc") {
                sys.cleanNode();
                std::cout << "Disconnected" << std::endl;
            } else cout << "No Command " << command[0] << " - Check Syntax" << endl;
        }
        #ifdef _WIN32
        con.bufferSave(2); // 儲存終端機內容
        #endif
    }
}