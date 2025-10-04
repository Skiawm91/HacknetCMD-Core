#include "os.h"
#include <tuple>
#include <string>
#include <vector>
using namespace std;

extern string playerName;
extern string playerIP;

tuple<string, string, int, vector<string>, vector<int>, bool> hnfcOS::Network::List() { // Name, Type, Ports, PortNames, PortNumber, Shell?
    if (parent->targetIP == playerIP) return { playerName + "'s PC", "DESKTOP", 6, vector<string>{"FTP", "HTTP", "SMTP" ,"SQL", "SSH", "SSL"}, vector<int>{21, 80, 25, 1433, 22, 443}, true };
    if (parent->targetIP == "4.31.168.192") return { "Hoyiqiang's NAS", "NAS", 1, vector<string>{"SSH"}, vector<int>{22}, false };
    return {"", "", 0, vector<string>{""}, vector<int>{0}, false};
}