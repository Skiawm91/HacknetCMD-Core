#include "os.h"
#include <string>
using std::string, std::vector;

void hnfcOS::Command::ChangeDir(HNCInterPreter::NodeInfo& node, const string& dir) {
    if (dir.empty()) return;

    vector<string> foldersToCd;
    size_t start = 0, end;
    while ((end = dir.find('/', start)) != string::npos) {
        if (end > start)
            foldersToCd.push_back(dir.substr(start, end - start));
        start = end + 1;
    }
    if (start < dir.size())
        foldersToCd.push_back(dir.substr(start));

    // 起始層級
    string currentPath = parent->path.empty() ? "/" : parent->path;
    HNCInterPreter::NodeInfo::FolderEntry* currentFolder = nullptr;

    // 找到目前所在的展開資料夾
    if (currentPath != "/") {
        vector<string> pathParts;
        start = 1; // skip '/'
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
            if (!found) return; // 當前 path 錯誤，直接退出
        }
        currentFolder = f;
    }

    // cd 命令處理
    vector<HNCInterPreter::NodeInfo::FolderEntry>* targetFolders = currentFolder ? &currentFolder->subfolders : &node.folders;

    for (auto &folderName : foldersToCd) {
        if (folderName == ".") continue; // 保持當前目錄
        else if (folderName == "..") {
            // 回上一層
            if (currentFolder) {
                currentFolder->expand = 0; // 收起當前資料夾
                size_t pos = currentPath.rfind('/', currentPath.length() - 2);
                if (pos != string::npos) currentPath.erase(pos + 1);
                else currentPath = "/";
                // 更新 currentFolder 到上一層
                currentFolder = nullptr;
                targetFolders = &node.folders;
                // 遍歷 path 找到上一層 folder
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
                currentPath = "/"; // 根目錄
            }
        } else {
            // 在當前資料夾下找要 cd 的資料夾
            bool found = false;
            for (auto &f : *targetFolders) {
                if (f.name == folderName) {
                    // 收起同層其他資料夾
                    for (auto &sibling : *targetFolders)
                        if (&sibling != &f) sibling.expand = 0;

                    f.expand = 2; // 已展開
                    currentPath = (currentPath.empty() ? "/" : currentPath) + f.name + "/";
                    currentFolder = &f;
                    targetFolders = &f.subfolders;
                    found = true;
                    break;
                }
            }
            if (!found) return; // 找不到資料夾就退出
        }
    }

    parent->path = currentPath; // 更新 path
    parent->displayChse = 3;
}