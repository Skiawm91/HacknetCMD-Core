#include "os.h"
#include "input.h"
#include "hnasm.h"
#include "console.h"
#include <iostream>
#include <vector>
using namespace std;

extern ManageInput mi;
extern Console con;
extern HNASM hnasm;
extern string playerIP;
extern string targetIP;

void hnfcOS::Display() {
    mi.kb.disable();
    cout << "\n\n";
    auto [Name, Type, Ports, portName, portNumber, Shell] = network.List();
    if (!Name.empty() && !Type.empty()) {
        hnasm.script("hnfcOS/display/icon.chns", "ICON_" + Type, vector<string>{"TARGETNAME", "TARGETIP"}, vector<string>{Name, targetIP});
    }
    cout << "\nYour IP: " + playerIP;
    if (!targetIP.empty()) cout << "\n[CONNECTED] " << Name << " (" << targetIP << ")";
    cout.flush();
    mi.async(3);
}