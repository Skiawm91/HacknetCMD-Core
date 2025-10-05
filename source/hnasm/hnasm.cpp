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

tuple<string, string, string, int, vector<string>, vector<int>, bool> HNASM::node(const string& fileName) {
    string scriptPath = "assets/nodes/" + fileName;
    ifstream file(scriptPath);
    string IP = "", Name = "", Type = "";
    int Ports = 0;
    vector<string> portNames = {};
    vector<int> portNumbers = {};
    bool Shell = false;
    string line, got;
    vector<string> command;
    while(getline(file, line)) {
        while(!line.empty() && line.front() == ' ') line.erase(0,1);
        istringstream iss(line);
        command.clear();
        iss >> got;
        command.push_back(got);
        if (command[0] == "NAME") {
            getline(iss, Name);
            Name.erase(0,1);
        } else {
            while(iss >> got) command.push_back(got);
            if (command[0] == "IP") IP = command[1];
            else if (command[0] == "TYPE") Type = command[1];
            else if (command[0] == "PORTS") try { Ports = stoi(command[1]); } catch(...) {}
            else if (command[0] == "PORTNAMES") {
                bool first = true;
                for (const auto &pName : command) {
                    if (!first) portNames.push_back(pName);
                    first = false;
                }
            }
            else if (command[0] == "PORTNUMBERS") {
                bool first = true;
                for (const auto &pNumber : command) {
                    try { if (!first) portNumbers.push_back(stoi(pNumber)); } catch (...) {}
                    first = false;
                }
            }
            else if (command[0] == "SHELL") {
                if (command[1] == "TRUE") Shell = true;
                else if (command[1] == "FALSE") Shell = false;
            }
        }
    }
    return { IP, Name, Type, Ports, portNames, portNumbers, Shell };
}