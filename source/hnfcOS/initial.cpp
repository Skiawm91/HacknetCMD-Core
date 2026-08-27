#define _HAS_STD_BYTE 0
#include "os.h"
#include "data.h"
#include "console.h"
#include "HNCIP.h"
#include "input.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

extern Data dta;
extern Console con;
extern hnfcOS os;
extern HNCInterPreter hncip;
extern ManageInput mi;

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

void hnfcOS::Initial(bool full) {
    extern string playerName;
    extern string playerLang;
    string lowerName;
    lowerName.resize(playerName.size());
    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
    dta.load("data/" + lowerName + "/save.hnd", vector<string>{"434F4E4649472E474F5449503A54525545"});
    if (dta.loaded) {
        if (dta.loadNumber == 0) playerIP = dta.load("data/" + lowerName + "/info.hnd", 2);
        else {
            playerIP = generatePublicIP();
            dta.replace("data/" + lowerName + "/info.hnd", 2, playerIP);
            dta.save("data/" + lowerName + "/save.hnd", "434F4E4649472E474F5449503A54525545");
        }
    } else {
        playerIP = generatePublicIP();
        dta.replace("data/" + lowerName + "/info.hnd", 2, playerIP);
        dta.save("data/" + lowerName + "/save.hnd", "434F4E4649472E474F5449503A54525545");
    }
    if (full) {
        dta.load("data/" + lowerName + "/save.hnd", vector<string>{"434F4E4649472E5455544F5249414C3A46414C5345"});\
        if (dta.loaded) {
            if (dta.loadNumber == 0) {
                hncip.script("terminal/initial.chns", "INITIAL");
                hncip.script("terminal/initial.chns", "HELPMSG");
            }
        } else {
            hncip.script("tutorial/failsafe.chns", "FAILSAFE_" + playerLang);
            hncip.script("terminal/initial.chns", "INITIAL");
            // hncip.script("terminal/initial.chns", "TUTORIAL");
            // s.Tutorial();
        }
        os.Interface();
    } else {
        con.clear(true);
        con.print("\n\n");
        con.setColorBg("0c1013").once().noFill(); // 先暫時這樣
        con.println("> connect " + playerIP).save();
        targetIP = playerIP;
        node = sys.getNode(targetIP);
        mi.kb.historyClear();
        os.Interface();
    }
}
