#include "hnasm/CHNScript.h"
#include "hnasm.h"
#include <sstream>
#include <string>
using namespace std;

extern HNASM hnasm;

void CHNScript::GOTO(const string& targetFile, const string& content) {
    istringstream iss(content);
    string partName;
    iss >> partName;
    hnasm.script(targetFile, partName);
}