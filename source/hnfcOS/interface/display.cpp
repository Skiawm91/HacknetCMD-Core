#include "interface.h"
#include "../../input/input.h"
#include "../../console/console.h"
#include <iostream>
using namespace std;

extern ManageInput mi;
extern Console con;

void Display() {
    cout << "DISPLAY ONCOMING...";
    mi.async(3);
}