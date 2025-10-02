#include "os.h"
#include "input.h"
#include "console.h"
#include <iostream>
using namespace std;

extern ManageInput mi;
extern Console con;
extern string playerIP;

void hnfcOS::Display() {
    mi.kb.disable();
    cout << "\n\nDISPLAY ONCOMING...";
    cout << "\nIP: " + playerIP;
    cout.flush();
    mi.async(3);
}