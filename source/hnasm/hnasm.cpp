#include "hnasm.h"
#include "scripts/ASMScript.h"
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
using namespace std;

extern string playerName;

void HNASM(const string& fileName, const string& partName) {
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
            HNScript s;
            istringstream got(line);
            got >> command;
            getline(got, content);
            if (!content.empty() && content[0] == ' ') {content = content.substr(1);}
            content = regex_replace(content, regex(R"(\$\{PLAYER\})"), playerName); // replace
            if (command=="WAIT") {s.WAIT(content);}
            else if (command=="CLEAR") {s.CLEAR();}
            else if (command=="PRINT") {s.PRINT(content);}
            else if (command=="PRINTR") {s.PRINTR(content);}
            else if (command=="PRINTWFW") {s.PRINTWFW(content);}
            else if (command=="PLAYAUDIO") {s.PLAYAUDIO(content);}
            else if (command=="GETINPUT") {s.GETINPUT(content);}
            else if (command=="GETINPUTR") {s.GETINPUTR(content);}
            else if (command=="GETINPUTPWD") {s.GETINPUTPWD(content);}
            else if (command=="GETINPUTPWDR") {s.GETINPUTPWDR(content);}
            else if (command=="GETINPUTNUL") {s.GETINPUTNUL(content);}
            else if (command=="GOTO") {s.GOTO(fileName, content);}
        }
    }
}