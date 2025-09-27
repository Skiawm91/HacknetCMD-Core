#include "config.h"
#include "../console/console.h"
#include <vector>
using namespace std;

extern Console con;

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
    con.resize(120 * (settings.cmdsize + 1), 30 * (settings.cmdsize + 1));
}