#include "data.h"
#include "console.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <vector>
using namespace std;

extern Console con;

static void VT100Enable(bool enable) {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD outMode;
    GetConsoleMode(hOut, &outMode);
    if (enable) {
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    } else {
        outMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }
    SetConsoleMode(hOut, outMode);
    #endif
}

void Data::Config::reload() {
    // VT100 Configuration (Load first)
    // VT100 Color
    parent->load("data/config.hnd", vector<string>{"VT100COLOR=0", "VT100COLOR=1"});
    if (parent->loaded) vt100Color = parent->loadNumber;
    // VT100 CMD Resize
    parent->load("data/config.hnd", vector<string>{"VT100CMDRESIZE=0", "VT100CMDRESIZE=1"});
    if (parent->loaded) vt100Resize = parent->loadNumber;
    parent->load("data/config.hnd", vector<string>{"TRUE24BITCOLOR=0", "TRUE24BITCOLOR=1"});
    if (parent->loaded) true24BitColor = parent->loadNumber;
    #ifdef _WIN32
    if (vt100Color == 1 || vt100Resize == 1) VT100Enable(true);
    else VT100Enable(false); 
    #endif
    // General
    // VERBOSE
    parent->load("data/config.hnd", vector<string>{"VERBOSEBOOT=0", "VERBOSEBOOT=1"});
    if (parent->loaded) verboseBoot = parent->loadNumber;
    // DYNAMICLOGO
    parent->load("data/config.hnd", vector<string>{"DYNAMICLOGO=0", "DYNAMICLOGO=1"});
    if (parent->loaded) dynamicLogo = parent->loadNumber;
    // LANGUAGE
    parent->load("data/config.hnd", vector<string>{"LANGUAGE=0", "LANGUAGE=1", "LANGUAGE=2"});
    if (parent->loaded) language = parent->loadNumber;
    // CMDSIZE
    parent->load("data/config.hnd", vector<string>{"CMDSIZE=0", "CMDSIZE=1", "CMDSIZE=2", "CMDSIZE=3"});
    if (parent->loaded) cmdsize = parent->loadNumber;
    con.resize(144 + 12 * cmdsize, 36 + 3 * cmdsize);
}