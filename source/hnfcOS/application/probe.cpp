#define _HAS_STD_BYTE 0
#include "os.h"
#include "console.h"
#include "HNCIP.h"
#include "input.h"
#include <thread>
#include <chrono>

extern Console con;
extern HNCInterPreter hncip;
extern ManageInput mi;

void hnfcOS::Application::Probe(HNCInterPreter::NodeInfo& node) {
    if (parent->Mode == "Display") {
        #ifdef _WIN32
        con.clear();
        #elif __APPLE__
        con.clearBuf2();
        #endif
        parent->MenuBar();
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
    hncip.script("hnfcOS/application/probe.chns", "TOPLINE" + ((parent->Mode == "Terminal") ? "_T" : string("")), vector<string>{"NODENAME", "NODEIP", "PORTS"}, vector<string>{node.Name, node.IP, to_string(node.Ports)});
    int i = 0;
    for (const auto &pN : node.portNames) {
        if (pN == "SSH") hncip.script("hnfcOS/application/probe.chns", "SSH" + ((parent->Mode == "Terminal") ? "_T" : string("")), vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "FTP") hncip.script("hnfcOS/application/probe.chns", "FTP" + ((parent->Mode == "Terminal") ? "_T" : string("")), vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "HTTP") hncip.script("hnfcOS/application/probe.chns", "HTTP" + ((parent->Mode == "Terminal") ? "_T" : string("")), vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "SMTP") hncip.script("hnfcOS/application/probe.chns", "SMTP" + ((parent->Mode == "Terminal") ? "_T" : string("")), vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "SSL") hncip.script("hnfcOS/application/probe.chns", "SSL" + ((parent->Mode == "Terminal") ? "_T" : string("")), vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        else if (pN == "SQL") hncip.script("hnfcOS/application/probe.chns", "SQL" + ((parent->Mode == "Terminal") ? "_T" : string("")), vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
        i++;
    }
    if (parent->Mode == "Terminal") hncip.script("hnfcOS/application/probe.chns", "ENDLINE", vector<string>{"PORTS"}, vector<string>{to_string(node.Ports)});
    else if (parent->Mode == "Display") mi.async(3);
}