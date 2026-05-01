#include "os.h"
#include <iostream>
#include <string>
#include <functional>
#include <vector>
using std::cout, std::endl, std::string, std::vector;

void hnfcOS::Command::ChangeDir(HNCInterPreter::NodeInfo& node, const string& dir) {
    if (dir.empty()) return;

    bool isAbsolute = (!dir.empty() && dir[0] == '/');

    vector<string> foldersToCd;
    size_t start = isAbsolute ? 1 : 0, end;
    while ((end = dir.find('/', start)) != string::npos) {
        if (end > start)
            foldersToCd.push_back(dir.substr(start, end - start));
        start = end + 1;
    }
    if (start < dir.size())
        foldersToCd.push_back(dir.substr(start));

    // 絕對路徑：從根目錄開始，收起所有展開的資料夾
    if (isAbsolute) {
        // 收起所有展開的資料夾
        function<void(vector<HNCInterPreter::NodeInfo::FolderEntry>&)> collapseAll;
        collapseAll = [&](vector<HNCInterPreter::NodeInfo::FolderEntry>& folders) {
            for (auto& f : folders) {
                f.expand = 0;
                collapseAll(f.subfolders);
            }
        };
        collapseAll(node.folders);
        parent->path = "/";
    }

    // 起始層級
    string currentPath = parent->path.empty() ? "/" : parent->path;
    HNCInterPreter::NodeInfo::FolderEntry* currentFolder = nullptr;

    // 找到目前所在的展開資料夾
    if (currentPath != "/") {
        vector<string> pathParts;
        start = 1;
        while ((end = currentPath.find('/', start)) != string::npos) {
            pathParts.push_back(currentPath.substr(start, end - start));
            start = end + 1;
        }

        HNCInterPreter::NodeInfo::FolderEntry* f = nullptr;
        vector<HNCInterPreter::NodeInfo::FolderEntry>* folders = &node.folders;

        for (auto &part : pathParts) {
            bool found = false;
            for (auto &cf : *folders) {
                if (cf.name == part) {
                    f = &cf;
                    folders = &cf.subfolders;
                    found = true;
                    break;
                }
            }
            if (!found) return;
        }
        currentFolder = f;
    }

    // cd 命令處理
    vector<HNCInterPreter::NodeInfo::FolderEntry>* targetFolders = currentFolder ? &currentFolder->subfolders : &node.folders;

    for (auto &folderName : foldersToCd) {
        if (folderName == ".") continue;
        else if (folderName == "..") {
            if (currentFolder) {
                currentFolder->expand = 0;
                size_t pos = currentPath.rfind('/', currentPath.length() - 2);
                if (pos != string::npos) currentPath.erase(pos + 1);
                else currentPath = "/";
                currentFolder = nullptr;
                targetFolders = &node.folders;
                if (currentPath != "/") {
                    vector<string> pathParts;
                    size_t s = 1, e;
                    while ((e = currentPath.find('/', s)) != string::npos) {
                        pathParts.push_back(currentPath.substr(s, e - s));
                        s = e + 1;
                    }
                    HNCInterPreter::NodeInfo::FolderEntry* f = nullptr;
                    vector<HNCInterPreter::NodeInfo::FolderEntry>* folders = &node.folders;
                    for (auto &part : pathParts) {
                        for (auto &cf : *folders) {
                            if (cf.name == part) {
                                f = &cf;
                                folders = &cf.subfolders;
                                break;
                            }
                        }
                    }
                    currentFolder = f;
                    targetFolders = currentFolder ? &currentFolder->subfolders : &node.folders;
                }
            } else {
                currentPath = "/";
            }
        } else {
            bool found = false;
            for (auto &f : *targetFolders) {
                if (f.name == folderName) {
                    for (auto &sibling : *targetFolders)
                        if (&sibling != &f) sibling.expand = 0;
                    f.expand = 2;
                    currentPath = (currentPath.empty() ? "/" : currentPath) + f.name + "/";
                    currentFolder = &f;
                    targetFolders = &f.subfolders;
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "Invalid Path" << endl;
                return;
            }
        }
    }

    parent->path = currentPath;
    parent->file = nullptr;
    parent->displayChse = 3;
}