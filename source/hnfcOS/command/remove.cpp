#include "os.h"
#include "HNCIP.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
using std::vector, std::string, std::cout, std::endl, std::stringstream;

extern HNCInterPreter hncip;

static bool resolvePath(
    const string& path, bool isAbsolute,
    const string& currentPath,
    HNCInterPreter::NodeInfo& node,
    vector<HNCInterPreter::NodeInfo::FolderEntry>*& outFolders,
    vector<HNCInterPreter::NodeInfo::FileEntry>*& outFiles)
{
    outFolders = &node.folders;
    outFiles = &node.files;

    vector<HNCInterPreter::NodeInfo::FolderEntry*> folderStack;
    if (!isAbsolute && !currentPath.empty() && currentPath != "/") {
        string p = currentPath;
        if (p.back() == '/') p.pop_back();
        stringstream ss(p);
        string part;
        vector<HNCInterPreter::NodeInfo::FolderEntry>* cur = &node.folders;
        while (getline(ss, part, '/')) {
            if (part.empty()) continue;
            for (auto& f : *cur) {
                if (f.name == part) {
                    folderStack.push_back(&f);
                    cur = &f.subfolders;
                    break;
                }
            }
        }
    }

    if (!path.empty()) {
        stringstream ss(path);
        string part;
        while (getline(ss, part, '/')) {
            if (part.empty() || part == ".") continue;
            if (part == "..") {
                if (!folderStack.empty()) folderStack.pop_back();
            } else {
                vector<HNCInterPreter::NodeInfo::FolderEntry>* cur = folderStack.empty() ? &node.folders : &folderStack.back()->subfolders;
                bool found = false;
                for (auto& f : *cur) {
                    if (f.name == part) {
                        folderStack.push_back(&f);
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }
        }
    }

    if (folderStack.empty()) {
        outFolders = &node.folders;
        outFiles = &node.files;
    } else {
        outFolders = &folderStack.back()->subfolders;
        outFiles = &folderStack.back()->files;
    }

    return true;
}

void hnfcOS::Command::Remove(const string& path, HNCInterPreter::NodeInfo& node) {
    bool isAbsolute = (!path.empty() && path[0] == '/');

    vector<string> parts;
    stringstream ss(path);
    string part;
    while (getline(ss, part, '/')) {
        if (part.empty() || part == ".") continue;
        parts.push_back(part);
    }

    string fileName = parts.back();
    parts.pop_back();

    string dirPath = "";
    for (const auto& p : parts) dirPath += p + "/";

    vector<HNCInterPreter::NodeInfo::FolderEntry>* targetFolders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* targetFiles;

    if (!resolvePath(dirPath, isAbsolute, parent->path, node, targetFolders, targetFiles)) {
        string displayPath = dirPath;
        if (!displayPath.empty() && displayPath.back() == '/') displayPath.pop_back();
        if (isAbsolute && !displayPath.empty() && displayPath[0] != '/') displayPath = "/" + displayPath;
        cout << "Invalid Path" << endl;
        cout << "Folder " << displayPath << " not found!" << endl;
        return;
    }

    // * 清空
    if (fileName == "*") {
        for (auto it = targetFiles->begin(); it != targetFiles->end();) {
            parent->MenuBar(); // 避免頂欄消失
            hncip.script("hnfcOS/command/remove.chns", "REMOVE", vector<string>{"FILENAME"}, vector<string>{it->name});
            it = targetFiles->erase(it);
        }
        parent->sys.saveNode(node);
        return;
    }

    // 刪除指定檔案
    for (auto it = targetFiles->begin(); it != targetFiles->end(); ++it) {
        if (it->name == fileName) {
            hncip.script("hnfcOS/command/remove.chns", "REMOVE", vector<string>{"FILENAME"}, vector<string>{it->name});
            targetFiles->erase(it);
            parent->sys.saveNode(node);
            return;
        }
    }

    cout << "File " << path << " not found!" << endl;
}