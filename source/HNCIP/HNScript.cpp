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
            // Print
            else if (command=="CLEAR") hncips.CLEAR();
            else if (command=="PRINT") hncips.PRINT(content);
            else if (command=="PRINTR") hncips.PRINTR(content);
            else if (command=="PRINTWFW") hncips.PRINTWFW(content);
            else if (command=="PRINTAT") hncips.PRINTAT(content);
            else if (command=="PRINTAT_NB") hncips.PRINTAT_NB(content);
            else if (command=="PRINTAT.NEOL") hncips.PRINTAT(content, false, true);
            else if (command=="PRINTAT.NEOL_NB") hncips.PRINTAT_NB(content, false, true);
            // Print w/Save
            else if (command=="PRINTS") hncips.PRINT(content, true);
            else if (command=="PRINTRS") hncips.PRINTR(content, true);
            else if (command=="PRINTWFWS") hncips.PRINTWFW(content, true);
            else if (command=="PRINTATS") hncips.PRINTAT(content, true);
            else if (command=="PRINTATS_NB") hncips.PRINTAT_NB(content, true);
            else if (command=="PRINTATS.NEOL") hncips.PRINTAT(content, true, true);
            else if (command=="PRINTATS.NEOL_NB") hncips.PRINTAT_NB(content, true, true);
            // Color
            else if (command=="COLOR") hncips.COLOR(content);
            else if (command=="COLORBG") hncips.COLORBG(content);
            else if (command=="COLORNF") hncips.COLOR(content, true);
            else if (command=="COLORBGNF") hncips.COLORBG(content, true);
            // Color (once)
            else if (command=="COLOR_ONCE") hncips.COLOR(content, false, true);
            else if (command=="COLORBG_ONCE") hncips.COLORBG(content, false, true);
            else if (command=="COLORNF_ONCE") hncips.COLOR(content, true, true);
            else if (command=="COLORBGNF_ONCE") hncips.COLORBG(content, true, true);
            // others
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