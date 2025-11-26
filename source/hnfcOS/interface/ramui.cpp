#define _HAS_STD_BYTE 0
#include "os.h"
#include "input.h"
#include "HNCIP.h"
#include "console.h"
#include "data.h"
#include <iostream>
#include <vector>
#include <filesystem>
using namespace std;

extern ManageInput mi;
extern Console con;
extern Data dta;
extern HNCInterPreter hncip;
extern string playerIP, playerLang;

void hnfcOS::RAMUI() {
    Mode = "RAMUI";
    con.cursor.hide();
    mi.kb.disable();
    cout << "\n\n";
    cout << "RAMUI is coming soon!";
    cout.flush();
    mi.async(3);
    // mi.mouse.btnDel(vector<string>{""});
    // mi.mouse.cbClean("DISPLAY");
}