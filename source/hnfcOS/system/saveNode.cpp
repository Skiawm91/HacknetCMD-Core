#include "os.h"
#include "HNCIP.h"
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

void hnfcOS::Kit::saveNode(const HNCInterPreter::NodeInfo& node) {
    ofstream f(node.filePath);
    if (!f.is_open()) return;

    f << "IP " << node.IP << "\n";
    f << "NAME " << node.Name << "\n";
    f << "TYPE " << node.Type << "\n";
    f << "PORTS " << node.Ports << "\n";

    if (!node.portNames.empty()) {
        f << "PORTNAMES";
        for (const auto& p : node.portNames) f << " " << p;
        f << "\n";
    }
    if (!node.portNumbers.empty()) {
        f << "PORTNUMBERS";
        for (const auto& p : node.portNumbers) f << " " << p;
        f << "\n";
    }

    f << "PROXY " << (node.Proxy ? "TRUE" : "FALSE") << "\n";
    f << "FIREWALL " << (node.Firewall ? "TRUE" : "FALSE") << "\n";
    f << "USER " << node.User << "\n";
    f << "PASSWD " << node.Passwd << "\n";
    f << "HACKSTATKEEP " << (node.hackStatKeep ? "TRUE" : "FALSE") << "\n";

    f << "FILESYSTEM\n";

    function<void(const HNCInterPreter::NodeInfo::FolderEntry&, int)> writeFolder;
    writeFolder = [&](const HNCInterPreter::NodeInfo::FolderEntry& folder, int indent) {
        string pad(indent * 2, ' ');
        f << pad << "FOLDER " << folder.name << "\n";
        for (const auto& sf : folder.subfolders) writeFolder(sf, indent + 1);
        for (const auto& fi : folder.files) {
            f << pad << "  FILE " << fi.name << "\n";
            for (const auto& line : fi.contents) {
                if (line != " ") f << pad << "    CONTENT " << line << "\n";
            }
            f << pad << "  END_FILE\n";
        }
        f << pad << "END_FOLDER\n";
    };

    for (const auto& folder : node.folders) writeFolder(folder, 1);
    for (const auto& fi : node.files) {
        f << "  FILE " << fi.name << "\n";
        for (const auto& line : fi.contents) {
            if (line != " ") f << "    CONTENT " << line << "\n";
        }
        f << "  END_FILE\n";
    }

    f << "END_FILESYSTEM\n";

    if (!node.scanIPs.empty()) {
        f << "SCANIPS";
        for (const auto& ip : node.scanIPs) f << " " << ip;
        f << "\n";
    }

    f << "TRACE " << node.Trace << "\n";
}