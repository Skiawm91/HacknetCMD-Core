#include "HNCIP.h"
#include <string>
#include <optional>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
using namespace std;

extern string playerName;

HNCInterPreter::NodeInfo HNCInterPreter::node(const string& fileName, const optional<vector<string>>& targetVar, const optional<vector<string>>& returnText, const string& filePath) {
    HNCInterPreter::NodeInfo node;
    string scriptPath = filePath + fileName;
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
            } else if (command[0] == "HACKSTATKEEP") {
                if (command[1] == "TRUE") node.hackStatKeep = true;
                else if (command[1] == "FALSE") node.hackStatKeep = false;
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
                            while (!line.empty() && line.front() == ' ') line.erase(0, 1);
                            if (line == "END_FILE") break;
                            istringstream iss(line);
                            command.clear();
                            while (iss >> got) command.push_back(got);
                            if (command[0] == "CONTENT") {
                                string content;
                                for (const auto &c : command) {
                                    if (c == "CONTENT") continue;
                                    content += " " + c;
                                }
                                if (!content.empty() && content.front() == ' ')
                                    content.erase(0, 1);

                                fileEntry.contents.push_back(content);
                            }
                        }
                        if (folderStack.empty())
                            node.files.push_back(std::move(fileEntry));
                        else
                            folderStack.back()->files.push_back(std::move(fileEntry));
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