#include "getIP.h"
#include "input/input.h"
#include "cmd.h"
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
using namespace std;

bool isPublicIP(int a, int b, int c, int d) {
    if (a == 10) return false;                 // 10.0.0.0/8
    if (a == 100 && b >= 64 && b <= 127) return false; // 100.64.0.0/10
    if (a == 127) return false;                // 127.0.0.0/8
    if (a == 169 && b == 254) return false;    // 169.254.0.0/16
    if (a == 172 && b >= 16 && b <= 31) return false; // 172.16.0.0/12
    if (a == 192 && b == 0 && c == 0) return false;   // 192.0.0.0/24
    if (a == 192 && b == 0 && c == 2) return false;   // 192.0.2.0/24 TEST-NET-1
    if (a == 192 && b == 88 && c == 99) return false; // 192.88.99.0/24
    if (a == 192 && b == 168) return false;    // 192.168.0.0/16
    if (a == 198 && (c == 18 || c == 19)) return false; // 198.18.0.0/15
    if (a == 198 && b == 51 && c == 100) return false;  // 198.51.100.0/24 TEST-NET-2
    if (a == 203 && b == 0 && c == 113) return false;   // 203.0.113.0/24 TEST-NET-3
    if (a >= 224) return false;                // Multicast & future
    return true;
}

std::string generatePublicIP() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);

    int a, b, c, d;
    do {
        a = dist(gen);
        b = dist(gen);
        c = dist(gen);
        d = dist(gen);
    } while (!isPublicIP(a, b, c, d));

    return std::to_string(a) + "." + std::to_string(b) + "." +
           std::to_string(c) + "." + std::to_string(d);
}

string ipAddress;

void GetIP() {
    extern string playerName;
    string lowerName;
    lowerName.resize(playerName.size());
    string line;
    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
    ifstream saveFile("config/" + lowerName + "/save.hnd");
    if (saveFile.is_open()) {
        while (getline(saveFile, line)) {
            if (line == "434F4E4649472E474F5449503A54525545") {
                ifstream ipFile("config/" + lowerName + "/ip.hnd");
                if (ipFile.is_open()) {
                    getline(ipFile, line);
                    ipAddress = line;
                } else {
                    ipAddress = "127.0.0.1";
                }
                Cmd();
                return;
            }
        }
    }
    while(true) {
        if (lowerName == "guest") {
            ipAddress = generatePublicIP();
            Cmd();
            return;
        } else {
            ofstream ipFile("config/" + lowerName + "/ip.hnd");
            if (ipFile.is_open()) {
                ipAddress = generatePublicIP();
                ipFile << ipAddress << endl;
                ipFile.close();
            }
            ofstream saveFile("config/" + lowerName + "/save.hnd");
            if (saveFile.is_open()) {
                saveFile << "434F4E4649472E474F5449503A54525545" << endl;
                saveFile.close();
            }
            Cmd();
            return;
        }
    }
}
