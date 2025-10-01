#include "hnasm.h"
#include "CHNScript.h"
#include <optional>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
using namespace std;

void HNASM(const string& fileName, const string& partName, const optional<string> targetVar, const optional<string> returnText) {
    string scriptPath = "assets/scripts/" + fileName;
    ifstream file(scriptPath);
    string line;
    string command, content;
    bool readcmd = partName.empty();
    while(getline(file, line)) {
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
            if (targetVar && returnText) content = regex_replace(content, regex("\\$\\{" + *targetVar + "\\}"), *returnText); // replace
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