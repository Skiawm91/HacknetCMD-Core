#define _HAS_STD_BYTE 0
#include "os.h"
#include "console.h"
#include "HNCIP.h"
#include "input.h"
#include <thread>
#include <chrono>
#include <functional>
#include <iostream>

extern Console con;
extern HNCInterPreter hncip;
extern ManageInput mi;

void hnfcOS::Application::Probe(HNCInterPreter::NodeInfo& node) {
    if (parent->Mode == "Display" && !Probed) {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        con.clear();
        parent->MenuBar();
        parent->termTasks.push_back([targetIP = parent->targetIP, path = parent->path, node = node](){
            if (!targetIP.empty()) {
                if (!path.empty()) con.println(targetIP + path + "> Probe").save();
                else con.println(targetIP + "@> Probe").save();
            } else con.println("> Probe").save();
            hncip.script("hnfcOS/application/probe.chns", "TOPLINE_T_NOWAIT", vector<string>{"NODENAME", "NODEIP", "PORTS"}, vector<string>{node.Name, node.IP, to_string(node.Ports)});
            int i = 0;
            for (const auto &pN : node.portNames) {
                if (pN == "SSH") hncip.script("hnfcOS/application/probe.chns", "SSH_T", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
                else if (pN == "FTP") hncip.script("hnfcOS/application/probe.chns", "FTP_T", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
                else if (pN == "HTTP") hncip.script("hnfcOS/application/probe.chns", "HTTP_T", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
                else if (pN == "SMTP") hncip.script("hnfcOS/application/probe.chns", "SMTP_T", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
                else if (pN == "SSL") hncip.script("hnfcOS/application/probe.chns", "SSL_T", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
                else if (pN == "SQL") hncip.script("hnfcOS/application/probe.chns", "SQL_T", vector<string>{"PORT"}, vector<string>{to_string(node.portNumbers[i])});
                i++;
            }
            hncip.script("hnfcOS/application/probe.chns", "ENDLINE", vector<string>{"PORTS"}, vector<string>{to_string(node.Ports)});
        });
    } else if (parent->Mode == "Display" && Probed) {
        con.clear();
        parent->MenuBar();
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
    Probed = true;
    if (parent->Mode == "Terminal") {
        hncip.script("hnfcOS/application/probe.chns", "ENDLINE", vector<string>{"PORTS"}, vector<string>{to_string(node.Ports)});
        parent->displayChse = 2;
    }
    else if (parent->Mode == "Display") {
        mi.mouse.btnAdd("BACK", 12, 2, 8, 1);
        mi.mouse.cbCreate("PROBE", [&](const string& btnName){
            if (btnName == "BACK") {
                Probed = false;
                parent->displayChse = 0;
            }
        });
        mi.sync(3);
        mi.mouse.btnDel(vector<string>{"BACK"});
        mi.mouse.cbClean("PROBE");
    }
}