#include "HNCIP/script.h"
#include "HNCIP.h"
#include <sstream>
#include <string>
using namespace std;

extern HNCInterPreter hncip;

void HNCIPScript::GOTO(const string& targetFile, const string& content) {
    istringstream iss(content);
    string partName;
    iss >> partName;
    hncip.script(targetFile, partName);
}