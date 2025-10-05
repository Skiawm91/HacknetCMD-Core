#define _HAS_STD_BYTE 0
#include "os.h"
#include "config.h"
#include "function.h"
#include "hnasm.h"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>
#include <ctime>
#include <vector>
using namespace std;

#ifndef _WIN32
inline void Sleep(const int& ms) {usleep(ms * 1000);}
#endif
extern Config cfg;
extern Function func;
extern hnfcOS os;
extern HNASM hnasm;
extern string playerName;

void hnfcOS::Boot() {
    func.audio.stop();
    if (cfg.settings.verbose) {
        hnasm.script("boot.chns", "BOOT");
        hnasm.script("boot.chns", "OSCONFIG");
        hnasm.script("boot.chns", "BOOTCFG");
        hnasm.script("boot.chns", "NETCFGX");
        hnasm.script("boot.chns", "XSERVER");
        hnasm.script("boot.chns", "COMPLETE");
    } else {
        srand((unsigned int)time(nullptr));
        hnasm.script("ui.chns", "LOGO2");
        hnasm.script("ui.chns", "NULL");
        string loading;
        string block = "=";
        string preLoading = "--------------------";
        string spaces(48, ' ');
        for (int i = 1; i <= 100; ++i) {
            if (i % 5 == 0) {
                loading += block;
                preLoading = preLoading.substr(0, 20 - i / 5);
                cout << "\r" << spaces << "[" << loading << preLoading << "]" << flush;
            }
            Sleep(150 + rand() % 11);
        }
    }
    cfg.data.save("config/booted.hnd", playerName);
    os.Initial(true);
    return;
}