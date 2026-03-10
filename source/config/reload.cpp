#include "data.h"
#include "console.h"
#include <vector>
using namespace std;

extern Console con;

void Data::Config::reload() {
    // VERBOSE
    parent->load("data/config.hnd", vector<string>{"VERBOSE=0", "VERBOSE=1"});
    if (parent->loaded) verbose = parent->loadNumber;
    // VERBOSE
    parent->load("data/config.hnd", vector<string>{"LOGO=0", "LOGO=1"});
    if (parent->loaded) logo = parent->loadNumber;
    // LANGUAGE
    parent->load("data/config.hnd", vector<string>{"LANGUAGE=0", "LANGUAGE=1", "LANGUAGE=2"});
    if (parent->loaded) language = parent->loadNumber;
    // CMDSIZE
    parent->load("data/config.hnd", vector<string>{"CMDSIZE=0", "CMDSIZE=1", "CMDSIZE=2", "CMDSIZE=3"});
    if (parent->loaded) cmdsize = parent->loadNumber;
    con.resize(144 + 12 * cmdsize, 36 + 3 * cmdsize + 1);
}