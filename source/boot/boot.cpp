#define _HAS_STD_BYTE 0
#include "boot.h"
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
using namespace std;

#ifndef _WIN32
inline void Sleep(const int& ms) {usleep(ms * 1000);}
#endif

void Boot() {
    StopAudio();
    bool verbose = true;
    if (verbose) {
        HNASM("boot.chns", "BOOT");
        HNASM("boot.chns", "OSCONFIG");
        HNASM("boot.chns", "BOOTCFG");
        HNASM("boot.chns", "NETCFGX");
        HNASM("boot.chns", "XSERVER");
        HNASM("boot.chns", "COMPLETE");
    } else {
        HNASM("ui.chns", "LOGO2");
        HNASM("ui.chns", "NULL");
        string loading;
        string preLoading = "--------------------";
        for (int i = 1; i <= 100; ++i) {
            if (i % 5 == 0) {
                loading += "=";
                preLoading = preLoading.substr(0, 20 - i / 5);
            }
        cout << "\r                                                [" << loading << preLoading << "]" << flush;
        Sleep(150);
    }
    }
    Initial();
    return;
}