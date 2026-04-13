#include "os.h"
#include "HNCIP.h"
#include <filesystem>
#include <vector>
#include <string>
using namespace std;

extern HNCInterPreter hncip;
extern string playerName, playerIP;

HNCInterPreter::NodeInfo hnfcOS::System::getNode(const std::string &targetIP) {
    vector<string> files;
    string lowerName;
    lowerName.resize(playerName.size());
    transform(playerName.begin(), playerName.end(), lowerName.begin(), ::tolower);
    // 連線過就使用config的node
    if (!filesystem::exists("data/" + lowerName + "/nodes")) filesystem::create_directory("data/" + lowerName + "/nodes");
    for (const auto& entry : filesystem::directory_iterator("data/" + lowerName + "/nodes")) {
        if (entry.is_regular_file() && entry.path().extension() == ".hnn") {
            files.push_back(entry.path().filename().string());
        }
    }
    for (const auto &f : files) {
        auto node = hncip.node(f, vector<string>{"PLAYERIP", "PLAYERNAME"}, vector<string>{playerIP, playerName}, "data/" + lowerName + "/nodes/");
        if (targetIP == node.IP) return node;
    }
    // 第一次連線將從assets獲取node
    files.clear();
    for (const auto& entry : filesystem::directory_iterator("assets/nodes")) {
        if (entry.is_regular_file() && entry.path().extension() == ".hnn") {
            files.push_back(entry.path().filename().string());
        }
    }
    for (const auto &f : files) {
        auto node = (hncip.node(f, vector<string>{"PLAYERIP", "PLAYERNAME"}, vector<string>{playerIP, playerName}));
        if (targetIP == node.IP) {
            if (node.IP == playerIP) filesystem::copy_file("assets/nodes/" + f, "data/" + lowerName + "/nodes/player.hnn", filesystem::copy_options::skip_existing);
            else filesystem::copy_file("assets/nodes/" + f, "data/" + lowerName + "/nodes/" + node.IP + ".hnn", filesystem::copy_options::skip_existing);
            node.filePath = "data/" + lowerName + "/nodes/" + f;
            return node;
        }
    }
    return HNCInterPreter::NodeInfo{};
}