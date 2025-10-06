#include "hnasm.h"
#include "hnasm/CHNScript.h"
#include <optional>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
using namespace std;

void HNASM::script(const string& fileName, const string& partName, const optional<vector<string>>& targetVar, const optional<vector<string>>& returnText) {
    string scriptPath = "assets/scripts/" + fileName;
    ifstream file(scriptPath);
    string line;
    string command, content;
    bool readcmd = partName.empty();
    while(getline(file, line)) {
        while(!line.empty() && line.front() == ' ') line.erase(0,1);
        if (line==("BEGIN_" + partName)) {readcmd=true;}
        if (!partName.empty() && line==("END_" + partName)) {break;}
        if (readcmd) {
            command.clear();
            content.clear();
            CHNScript chns;
            istringstream got(line);
            got >> command;
            getline(got, content);
            if (!content.empty() && content[0] == ' ') {content = content.substr(1);}
            if (targetVar && returnText) {
                int i = 0;
                for (const auto &tv : *targetVar) {
                    int i2 = 0;
                    for (const auto &rt : *returnText) {
                        if (i == i2) content = regex_replace(content, regex("\\$\\{" + tv + "\\}"), rt); // replace
                        ++i2;
                    }
                    ++i;
                }
            }
            if (command=="WAIT") {chns.WAIT(content);}
            else if (command=="CLEAR") {chns.CLEAR();}
            else if (command=="PRINT") {chns.PRINT(content);}
            else if (command=="PRINTR") {chns.PRINTR(content);}
            else if (command=="PRINTWFW") {chns.PRINTWFW(content);}
            else if (command=="PRINTAT") {chns.PRINTAT(content);}
            else if (command=="PLAYAUDIO") {chns.PLAYAUDIO(content);}
            else if (command=="GETINPUT") {chns.GETINPUT(content);}
            else if (command=="GETINPUTR") {chns.GETINPUTR(content);}
            else if (command=="GETINPUTPWD") {chns.GETINPUTPWD(content);}
            else if (command=="GETINPUTPWDR") {chns.GETINPUTPWDR(content);}
            else if (command=="GETINPUTNUL") {chns.GETINPUTNUL(content);}
            else if (command=="GOTO") {chns.GOTO(fileName, content);}
        }
    }
}

HNASM::NodeInfo HNASM::node(const string& fileName, const optional<vector<string>>& targetVar, const optional<vector<string>>& returnText) {
    HNASM::NodeInfo node;
    string scriptPath = "assets/nodes/" + fileName;
    ifstream file(scriptPath);
    string line, got;
    vector<string> command;
    while(getline(file, line)) {
        while(!line.empty() && line.front() == ' ') line.erase(0,1);
        istringstream iss(line);
        command.clear();
        iss >> got;
        command.push_back(got);
        if (command[0] == "NAME") {
            getline(iss, node.Name);
            if (targetVar && returnText) {
                int i = 0;
                for (const auto &tv : *targetVar) {
                    int i2 = 0;
                    for (const auto &rt : *returnText) {
                        if (i == i2) node.Name = regex_replace(node.Name, regex("\\$\\{" + tv + "\\}"), rt); // replace
                        ++i2;
                    }
                    ++i;
                }
            }
            node.Name.erase(0,1);
        } else {
            while(iss >> got) command.push_back(got);
            if (command[0] == "IP") {
                node.IP = command[1];
                if (targetVar && returnText) {
                    int i = 0;
                    for (const auto &tv : *targetVar) {
                        int i2 = 0;
                        for (const auto &rt : *returnText) {
                            if (i == i2) node.IP = regex_replace(node.IP, regex("\\$\\{" + tv + "\\}"), rt); // replace
                            ++i2;
                        }
                        ++i;
                    }
                }
            } else if (command[0] == "TYPE") node.Type = command[1];
            else if (command[0] == "PORTS") try { node.Ports = stoi(command[1]); } catch(...) {}
            else if (command[0] == "PORTNAMES") {
                bool first = true;
                for (const auto &pName : command) {
                    if (!first) node.portNames.push_back(pName);
                    first = false;
                }
            } else if (command[0] == "PORTNUMBERS") {
                bool first = true;
                for (const auto &pNumber : command) {
                    try { if (!first) node.portNumbers.push_back(stoi(pNumber)); } catch (...) {}
                    first = false;
                }
            } else if (command[0] == "USER") {
                node.User = command[1];
                if (targetVar && returnText) {
                    int i = 0;
                    for (const auto &tv : *targetVar) {
                        int i2 = 0;
                        for (const auto &rt : *returnText) {
                            if (i == i2) node.Name = regex_replace(node.Name, regex("\\$\\{" + tv + "\\}"), rt); // replace
                            ++i2;
                        }
                        ++i;
                    }
                }
            } else if (command[0] == "PASSWD") {
                node.Passwd = command[1];
                if (targetVar && returnText) {
                    int i = 0;
                    for (const auto &tv : *targetVar) {
                        int i2 = 0;
                        for (const auto &rt : *returnText) {
                            if (i == i2) node.Name = regex_replace(node.Name, regex("\\$\\{" + tv + "\\}"), rt); // replace
                            ++i2;
                        }
                        ++i;
                    }
                }
            } else if (command[0] == "ADMINKEEP") {
                if (command[1] == "TRUE") node.adminKeep = true;
                else if (command[1] == "FALSE") node.adminKeep = false;
            }
            else if (command[0] == "FILESYSTEM") {
                std::vector<NodeInfo::FolderEntry*> folderStack;
                while (getline(file, line)) {
                    if (line == "END_FILESYSTEM") break;
                    while (!line.empty() && line.front() == ' ') line.erase(0, 1);
                    istringstream iss(line);
                    command.clear();
                    while (iss >> got) command.push_back(got);
                    if (command.empty()) continue;
                    if (command[0] == "FOLDER") {
                        NodeInfo::FolderEntry newFolder;
                        newFolder.name = command[1];

                        if (folderStack.empty())
                            node.folders.push_back(std::move(newFolder)),
                            folderStack.push_back(&node.folders.back());
                        else
                            folderStack.back()->subfolders.push_back(std::move(newFolder)),
                            folderStack.push_back(&folderStack.back()->subfolders.back());
                    }
                    else if (command[0] == "END_FOLDER") {
                        if (!folderStack.empty()) folderStack.pop_back();
                    }
                    else if (command[0] == "FILE") {
                        NodeInfo::FileEntry fileEntry;
                        fileEntry.name = command[1];
                        while (getline(file, line)) {
                            if (line == "END_FILE") break;
                            while (!line.empty() && line.front() == ' ') line.erase(0, 1);
                            istringstream iss(line);
                            command.clear();
                            while (iss >> got) command.push_back(got);
                            if (command[0] == "WRITE") fileEntry.contents.push_back(command[1]);
                        }
                        if (!folderStack.empty()) folderStack.back()->files.push_back(std::move(fileEntry));
                    }
                }
            } else if (command[0] == "PROXY") {
                if (command[1] == "TRUE") node.Proxy = true;
                else if (command[1] == "FALSE") node.Proxy = false;
            } else if (command[0] == "FIREWALL") {
                if (command[1] == "TRUE") node.Firewall = true;
                else if (command[1] == "FALSE") node.Firewall = false;
            } else if (command[0] == "SCANIPS") {
                if (command[1] != "0") {
                    bool first = true;
                    for (const auto &sIP : command) {
                        try { if (!first) node.scanIPs.push_back(stoi(sIP)); } catch (...) {}
                        first = false;
                    }
                }
            } else if (command[0] == "TRACE") try { node.Trace = stoi(command[1]); } catch (...) {}
        }
    }
    return node;
}