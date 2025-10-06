#include "os.h"
#include "hnasm.h"
#include <filesystem>
#include <vector>
#include <string>
using namespace std;

extern HNASM hnasm;
extern string playerName, playerIP;

HNASM::NodeInfo hnfcOS::getNode() {
    vector<string> files;
    for (const auto& entry : filesystem::directory_iterator("assets/nodes")) {
        if (entry.is_regular_file() && entry.path().extension() == ".hnn") {
            files.push_back(entry.path().filename().string());
        }
    }
    for (const auto &f : files) {
        auto node = (hnasm.node(f, vector<string>{"PLAYERIP", "PLAYERNAME"}, vector<string>{playerIP, playerName}));
        if (targetIP == node.IP) return node;
    }
    return HNASM::NodeInfo{};
}