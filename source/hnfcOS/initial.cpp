#include "os.h"
#include "../config/config.h"
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

extern Config cfg;
extern hnfcOS os;

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

string generatePublicIP() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 255);

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

string playerIP;

void hnfcOS::Initial() {
    extern string playerName;
    string lowerName;
    lowerName.resize(playerName.size());
    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
    cfg.data.load("config/" + lowerName + "/save.hnd", vector<string>{"434F4E4649472E474F5449503A54525545"});
    if (cfg.data.loaded) {
        if (cfg.data.loadNumber == 0) {
            playerIP = cfg.data.load("config/" + lowerName + "/imfo.hnd", 2);
            os.Interface();
            return;
        }
    } else {
        playerIP = generatePublicIP();
        cfg.data.save("config/" + lowerName + "/info.hnd", playerIP);
        cfg.data.save("config/" + lowerName + "/save.hnd", "434F4E4649472E474F5449503A54525545");
        os.Interface();
        return;
    }
}
