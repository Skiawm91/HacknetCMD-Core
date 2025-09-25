#include "interface.h"
#include "../../input/input.h"
#include "../../function/function.h"
#include <iostream>
using namespace std;

extern ManageInput mi;
extern Function func;

void Display() {
    func.cmd.clear();
    cout << "DISPLAY ONCOMING...";
    mi.async(3);
}