#include "HNCIP.h"
#include "HNCIP/script.h"
#include <optional>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
using namespace std;

void HNCInterPreter::script(const string& fileName, const string& partName, const optional<vector<string>>& targetVar, const optional<vector<string>>& returnText, const string& filePath) {
    string scriptPath = filePath + fileName;
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
            HNCIPScript hncips;
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
            if (command=="WAIT") hncips.WAIT(content);
            else if (command=="CLEAR") hncips.CLEAR();
            else if (command=="PRINT") hncips.PRINT(content);
            else if (command=="PRINTR") hncips.PRINTR(content);
            else if (command=="PRINTWFW") hncips.PRINTWFW(content);
            else if (command=="PRINTAT") hncips.PRINTAT(content);
            else if (command=="PRINTAT_NB") hncips.PRINTAT_NB(content);
            // Print w/Save
            else if (command=="PRINTS") hncips.PRINT(content, true);
            else if (command=="PRINTRS") hncips.PRINTR(content, true);
            else if (command=="PRINTWFWS") hncips.PRINTWFW(content, true);
            else if (command=="PRINTATS") hncips.PRINTAT(content, true);
            else if (command=="PRINTATS_NB") hncips.PRINTAT_NB(content, true);
            else if (command=="PLAYAUDIO") hncips.PLAYAUDIO(content);
            else if (command=="GETINPUT") hncips.GETINPUT(content);
            else if (command=="GETINPUTR") hncips.GETINPUTR(content);
            else if (command=="GETINPUTPWD") hncips.GETINPUTPWD(content);
            else if (command=="GETINPUTPWDR") hncips.GETINPUTPWDR(content);
            else if (command=="GETINPUTNUL") hncips.GETINPUTNUL(content);
            else if (command=="GOTO") hncips.GOTO(fileName, content);
        }
    }
}