#define _HAS_STD_BYTE 0
#include "logUI.h"
#include "input/input.h"
#include "hnasm/hnasm.h"
#include "crypto/crypto.h"
#include "boot/boot.h"
#include "cmd.h"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <fstream>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <thread>
#include <atomic>
using namespace std;

string playerName;
extern ManageInput mi;

void LogUI() {
    string name;
    extern string input;
    extern string pwdtext;
    extern string shatext;
    mi.keyEnable();
    int chse;
    string shapwd, tgshapwd;
    while(true) {
        chse = 0;
        HNASM("ui.chns", "LOGO");
        HNASM("ui.chns", "USER");
        mi.btnAdd("LOGIN", 2, 9, 20, 3);
        mi.btnAdd("REGISTER", 2, 12, 20, 3);  
        mi.btnAdd("GUEST", 2, 15, 20, 3);
        mi.btnAdd("BACK", 2, 18, 20, 3);
        mi.cbCreate([&](const string& btnName) {
            if (btnName == "LOGIN") {
                chse = 1;
                mouseSync = false;
            }
            if (btnName == "REGISTER") {
                chse = 2;
                mouseSync = false;
            }
            if (btnName == "GUEST") {
                chse = 3;
                mouseSync = false;
            }
            if (btnName == "BACK") {
                chse = 4;
                mouseSync = false;
            }
        });
        mouseSync = true;
        while (true) {
            if (!mouseSync || !enterDetected || !escDetected) {
                mouseSync = false;
                enterDetected = false;
                escDetected = false;
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        mi.btnDel(vector<string>{"LOGIN", "REGISTER", "GUEST", "BACK"});
        mi.cbClean();
        switch(chse) {
            case 1:
                while(true) {
                    HNASM("logUI/login.chns", "NAME");
                    if (escDetected) break;
                    transform(input.begin(), input.end(), input.begin(), ::tolower);
                    name = input;
                    {
                        ifstream file("config/" + name + "/pw.hnud");
                        if (!file) {HNASM("logUI/login.chns", "ERROR");}
                        else {
                            string line;
                            getline(file, line);
                            istringstream iss(line);
                            iss >> tgshapwd;
                            HNASM("logUI/login.chns", "PASSWD");
                            while(!escDetected || !enterDetected);
                            escDetected = false;
                            enterDetected = false;
                            SHA256Encrypt(input);
                            shapwd = shatext;
                            if (shapwd == tgshapwd) {
                                ifstream file("config/" + name + "/name.hund");
                                if (file) {
                                    string line;
                                    getline(file, line);
                                    istringstream iss(line);
                                    iss >> playerName;
                                }
                                Boot();
                                return;
                            } else {
                                HNASM("logUI/login.chns", "ERROR");
                            }
                        }
                    }
                }
                break;
            case 2:
                {
                    string pwd[2];
                    while (true) {
                        HNASM("logUI/register.chns", "REGISTER");
                        HNASM("logUI/register.chns", "NAME");
                        inputMasked = false;
                        while(!escDetected || !enterDetected);
                        escDetected = false;
                        enterDetected = false;
                        transform(input.begin(), input.end(), input.begin(), ::tolower);
                        name = input;
                        HNASM("logUI/register.chns", "PASSWD");
                        while(!escDetected || !enterDetected);
                        inputMasked = true;
                        escDetected = false;
                        enterDetected = false;
                        pwd[0] = input;
                        HNASM("logUI/register.chns", "CONFIRM");
                        inputMasked = true;
                        while(!escDetected || !enterDetected);
                        escDetected = false;
                        enterDetected = false;
                        pwd[1] = input;
                        HNASM("logUI/register.chns", "DETAILS");
                        while(true) {
                            HNASM("logUI/register.chns", "CONFIRM2");
                            inputMasked = false;
                            while(!escDetected || !enterDetected);
                            escDetected = false;
                            enterDetected = false;
                            try {chse = stoi(input);} catch (const invalid_argument) {chse = 0;}
                            if (chse == 2) {
                                break;
                            } else {
                                if (!filesystem::exists("config")) {
                                    filesystem::create_directory("config");
                                }
                                    filesystem::create_directory("config/" + name);
                                    ofstream file("config/" + name + "/name.hund");
                                    if (file.is_open()) {
                                        file << name << endl;
                                        file.close();
                                    }
                                    if (pwd[0] == pwd[1]) {
                                    SHA256Encrypt(pwd[1]);
                                    shapwd = shatext;
                                    ofstream file("config/" + name + "/pw.hnud");
                                    if (file.is_open()) {
                                        file << shapwd << endl;
                                        file.close();
                                    }
                                    return;
                                } else {
                                    HNASM("logUI/register.chns", "INVCONFIRM");
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            case 3:
                Boot();
                return;
            case 4:
                return;
            default:
                break;
        }
    }
}