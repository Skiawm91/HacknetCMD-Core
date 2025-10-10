#include "os.h"
#include "HNCIP.h"
#include <filesystem>
#include <vector>
#include <string>
using namespace std;

extern HNCInterPreter hncip;
extern string playerName, playerIP;

HNCInterPreter::NodeInfo hnfcOS::getNode() {
    vector<string> files;
    for (const auto& entry : filesystem::directory_iterator("assets/nodes")) {
        if (entry.is_regular_file() && entry.path().extension() == ".hnn") {
            files.push_back(entry.path().filename().string());
        }
    }
    for (const auto &f : files) {
        auto node = (hncip.node(f, vector<string>{"PLAYERIP", "PLAYERNAME"}, vector<string>{playerIP, playerName}));
        if (targetIP == node.IP) return node;
    }
    return HNCInterPreter::NodeInfo{};
}