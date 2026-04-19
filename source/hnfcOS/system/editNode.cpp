#include "os.h"
#include "HNCIP.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

void hnfcOS::System::editNode(const string& key, const string& value) {
    // 更新記憶體
    if (key == "IP") parent->node.IP = value;
    else if (key == "NAME") parent->node.Name = value;
    else if (key == "TYPE") parent->node.Type = value;
    else if (key == "PORTS") try { parent->node.Ports = stoi(value); } catch (...) {}
    else if (key == "USER") parent->node.User = value;
    else if (key == "PASSWD") parent->node.Passwd = value;
    else if (key == "PROXY") parent->node.Proxy = (value == "TRUE");
    else if (key == "FIREWALL") parent->node.Firewall = (value == "TRUE");
    else if (key == "HACKSTATKEEP") parent->node.hackStatKeep = (value == "TRUE");
    else if (key == "TRACE") try { parent->node.Trace = stoi(value); } catch (...) {}
    else if (key == "SCANIPS") {
        parent->node.scanIPs.clear();
        istringstream iss(value);
        string ip;
        while (iss >> ip) {
            try { parent->node.scanIPs.push_back(ip); } catch (...) {}
        }
    }
    else if (key == "PORTNAMES") {
        parent->node.portNames.clear();
        istringstream iss(value);
        string p;
        while (iss >> p) parent->node.portNames.push_back(p);
    }
    else if (key == "PORTNUMBERS") {
        parent->node.portNumbers.clear();
        istringstream iss(value);
        string p;
        while (iss >> p) try { parent->node.portNumbers.push_back(stoi(p)); } catch (...) {}
    }

    // 更新檔案
    ifstream fin(parent->node.filePath);
    if (!fin.is_open()) return;

    vector<string> lines;
    string line;
    while (getline(fin, line)) lines.push_back(line);
    fin.close();

    bool found = false;
    for (auto& l : lines) {
        istringstream iss(l);
        string token;
        iss >> token;
        if (token == key) {
            l = key + " " + value;
            found = true;
            break;
        }
    }

    if (!found) lines.push_back(key + " " + value);

    ofstream fout(parent->node.filePath);
    for (const auto& l : lines) fout << l << "\n";
}