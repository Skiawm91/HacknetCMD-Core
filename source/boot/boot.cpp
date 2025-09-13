#define _HAS_STD_BYTE 0
#include "boot.h"
#include "../hnasm/hnasm.h"
#include "../getIP.h"
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <ctime>
using namespace std;

#ifndef _WIN32
inline void Sleep(const int& ms) {usleep(ms * 1000);}
#endif

void Boot() {
    srand((unsigned int)time(nullptr));
    HNASM("ui.chns", "LOGO");
    HNASM("ui.chns", "NULL");
    string block = "=";
    string loading = "";
    for (int i = 1; i <= 100; ++i) {
        if (i % 10 == 0) {loading += block;}
        if (i == 100) {cout << "\rProgress: [" << loading << "] " << i << "% " << flush;}
        else {cout << "\rProgress: [" << loading << ">" "] " << i << "% " << flush;}
        Sleep(rand() % 501);
    }
    HNASM("boot.chns", "BOOT");
    GetIP();
    return;
}