#include "misc.h"
#include <string>
using namespace std;

string Misc::toLangName(const int langCode) {
    if (langCode == 0) return "EN";
    if (langCode == 1) return "CHT";
    if (langCode == 2) return "CUSTOM";
    return "EN";
}