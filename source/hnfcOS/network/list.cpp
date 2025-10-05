#include "os.h"
#include "hnasm.h"
#include <tuple>
#include <string>
#include <vector>
#include <filesystem>
using namespace std;

extern HNASM hnasm;
extern string playerName;
extern string playerIP;

tuple<string, string, int, vector<string>, vector<int>, bool> hnfcOS::Network::List() { // Name, Type, Ports, PortNames, PortNumber, Shell?
    if (parent->targetIP == playerIP) return { playerName + "'s PC", "DESKTOP", 6, vector<string>{"FTP", "HTTP", "SMTP" ,"SQL", "SSH", "SSL"}, vector<int>{21, 80, 25, 1433, 22, 443}, true };
    if (parent->targetIP == "192.168.0.11") return { "Test Desktop", "DESKTOP", 1, vector<string>{"SSH"}, vector<int>{22}, false };
    if (parent->targetIP == "192.168.0.12") return { "Test Laptop", "LAPTOP", 1, vector<string>{"SSH"}, vector<int>{22}, false };
    if (parent->targetIP == "192.168.0.13") return { "Test NAS", "NAS", 1, vector<string>{"SSH"}, vector<int>{22}, false };
    if (parent->targetIP == "192.168.0.14") return { "Test Server", "SERVER", 1, vector<string>{"SSH"}, vector<int>{22}, false };
    if (parent->targetIP == "192.168.0.15") return { "Test Phone", "PHONE", 1, vector<string>{"SSH"}, vector<int>{22}, false };
    vector<string> files;
    for (const auto& entry : filesystem::directory_iterator("assets/nodes")) {
        if (entry.is_regular_file() && entry.path().extension() == ".hnn") {
            files.push_back(entry.path().filename().string());
        }
    }
    for (const auto &f : files) {
        auto [IP, Name, Type, Ports, portNames, portNumbers, Shell] = hnasm.node(f);
        if (parent->targetIP == IP) return { Name, Type, Ports, portNames, portNumbers, Shell };
    }
    return {"", "", 0, {}, {}, false};
}