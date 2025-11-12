#pragma once
#include <string>
#include <optional>
#include <vector>
#include <tuple>
using namespace std;

class HNCInterPreter {
public:
    void script(const string& fileName, const string& partName, const optional<vector<string>>& targetVar = nullopt, const optional<vector<string>>& returnText = nullopt);
    struct NodeInfo {
        struct FileEntry {
            string name;
            vector<string> contents;
        };
        struct FolderEntry {
            bool expand = false;
            string name;
            vector<FileEntry> files;
            vector<FolderEntry> subfolders;
        };
        string Name, IP, Type, User, Passwd;
        int Ports = 0, Trace = 0;
        bool hackStatKeep = true, Proxy = false, Firewall = false;
        vector<string> portNames;
        vector<int> portNumbers, scanIPs;
        vector<FolderEntry> folders; // <-- 新的巢狀 filesystem
    };
    NodeInfo node(const string& fileName, const optional<vector<string>>& targetVar = nullopt, const optional<vector<string>>& returnText = nullopt, const bool readConfig = false);
private:
};