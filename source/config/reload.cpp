#include "config.h"
#include <vector>
using namespace std;

void Config::reload() {
    data.load("config/config.hnd", vector<string>{"VERBOSE=0", "VERBOSE=1"}); // VERBOSE
    if (data.loaded) settings.verbose = data.loadNumber;
    data.load("config/config.hnd", vector<string>{"LANGUAGE=0", "LANGUAGE=1", "LANGUAGE=2"}); // LANGUAGE
    if (data.loaded) settings.language = data.loadNumber;
}