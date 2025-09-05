#include "noobornonoob.h"
#include "cmd.h"
#include "tutorial.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
using namespace std;

string ipAddress;

void Noobornonoob() {
    extern string playerName;
    string lowerName;
    string line;
    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
    istringstream iss("config/" + lowerName + "save.hnd");
    while(getline(iss, line)) {
        if (line == "CONFIG,IMNOTNOOB,TRUE") {
            Cmd();
        }
    }
    string sel;
    while(true) {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
        cout << "Welcome to Hacknet CMD!" << endl;
        cout << "If you never played this, type \"y\", else type \"n\"" << endl;
        cout << "type: ";
        cin >> sel;
        if (sel=="n") {
            ofstream file("config/" + lowerName + "save.hnd");
            if (file.is_open()) {
                cout << "CONFIG,IMNOTNOOB,TRUE" << endl;
                file.close();
            }
            ofstream file("config/" + lowerName + "ip.hnd");
            if (file.is_open()) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dist2(64, 127);
                uniform_int_distribution<> dist34(0, 255);
                cout << 100 << "." << dist2(gen) << "." << dist34(gen) << "." << dist34(gen) << endl;
            }
            Cmd();
            return;
        } else if (sel=="y") {
            Tutorial();
            return;
        }
    }
}