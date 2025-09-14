#define _HAS_STD_BYTE 0
#include "CHNScript.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string>
#include <sstream>
using namespace std;

#ifndef _WIN32
inline void Sleep(const int& ms) {usleep(ms * 1000);}
#endif

void CHNScript::WAIT(const string& content) {
    string arg[2];
    istringstream args(content);
    args >> arg[0] >> arg[1];
    try {
        double seconds = stod(arg[0]);
        Sleep(static_cast<int>(seconds * 1000));
    } catch (...) {}
}