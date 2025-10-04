#include "os.h"
#include "input.h"
#include "console.h"
#include <iostream>
using namespace std;

extern ManageInput mi;
extern Console con;
extern string playerIP;
extern string targetIP;

void hnfcOS::Display() {
    mi.kb.disable();
    cout << "\n\nDISPLAY ONCOMING...";
    cout << "\nYour IP: " + playerIP;
    if (!targetIP.empty()) cout << "\n[CONNECTED] " << targetIP;
    cout.flush();
    mi.async(3);
}