#include "data.h"
#include "console.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <vector>
using namespace std;

extern Console con;

void Data::Config::reload() {
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
    parent->load("data/config.hnd", vector<string>{"VT100COLOR=0", "VT100COLOR=1"});
    if (parent->loaded) vt100Color = parent->loadNumber;
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (vt100Color == 1) {
        DWORD outMode;
        GetConsoleMode(hOut, &outMode);
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, outMode);
    } else {
        DWORD outMode;
        GetConsoleMode(hOut, &outMode);
        outMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, outMode);
    }
    #endif
}