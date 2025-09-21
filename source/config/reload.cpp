#include "config.h"
#include "../function/function.h"
#include <vector>
using namespace std;

extern Function func;

void Config::reload() {
    // VERBOSE
    data.load("config/config.hnd", vector<string>{"VERBOSE=0", "VERBOSE=1"});
    if (data.loaded) settings.verbose = data.loadNumber;
    // LANGUAGE
    data.load("config/config.hnd", vector<string>{"LANGUAGE=0", "LANGUAGE=1", "LANGUAGE=2"});
    if (data.loaded) settings.language = data.loadNumber;
    // CMDSIZE
    data.load("config/config.hnd", vector<string>{"CMDSIZE=0", "CMDSIZE=1", "CMDSIZE=2", "CMDSIZE=3"});
    if (data.loaded) settings.cmdsize = data.loadNumber;
    if (settings.cmdsize == 0) func.cmd.resize(120, 30);
    else if (settings.cmdsize == 1) func.cmd.resize(240, 60);
    else if (settings.cmdsize == 2) func.cmd.resize(360, 90);
    else if (settings.cmdsize == 3) func.cmd.resize(480, 120);
}