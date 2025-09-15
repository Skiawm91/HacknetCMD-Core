#define _HAS_STD_BYTE 0
#include "boot.h"
#include "../misc/config.h"
#include "../audio.h"
#include "../hnasm/hnasm.h"
#include "../initial.h"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>
#include <ctime>
using namespace std;

#ifndef _WIN32
inline void Sleep(const int& ms) {usleep(ms * 1000);}
#endif
extern Config cfg;

void Boot() {
    StopAudio();
    if (cfg.settings.verbose) {
        HNASM("boot.chns", "BOOT");
        HNASM("boot.chns", "OSCONFIG");
        HNASM("boot.chns", "BOOTCFG");
        HNASM("boot.chns", "NETCFGX");
        HNASM("boot.chns", "XSERVER");
        HNASM("boot.chns", "COMPLETE");
    } else {
        srand((unsigned int)time(nullptr));
        HNASM("ui.chns", "LOGO2");
        HNASM("ui.chns", "NULL");
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
    Initial();
    return;
}