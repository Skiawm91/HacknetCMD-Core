#include "os.h"
#include <vector>
#include <string>
#include <iostream>
using std::vector, std::string;

void hnfcOS::Command::Concatenate(const string &targetName, HNCInterPreter::NodeInfo& node) {
    string currentPath = parent->path.empty() ? "/" : parent->path;

    // 判斷目前 folder（root = nullptr）
    HNCInterPreter::NodeInfo::FolderEntry* currentFolder = nullptr;
    vector<HNCInterPreter::NodeInfo::FolderEntry>* folderList = &node.folders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* fileList = &node.files;
    vector<string>* fileContents;

    // 如果不是 root，就往下找
    if (currentPath != "/") {
        vector<string> parts;
        size_t start = 1, end;

        while ((end = currentPath.find('/', start)) != string::npos) {
            parts.push_back(currentPath.substr(start, end - start));
            start = end + 1;
        }

        for (auto& part : parts) {
            bool found = false;

            for (auto& f : *folderList) {
                if (f.name == part) {
                    currentFolder = &f;
                    folderList = &f.subfolders;
                    fileList = &f.files;
                    for (auto &f : f.files) {
                        if (f.name == targetName) {
                            fileContents = &f.contents;
                            f.opened = true;
                            found = true;
                            break;
                        }
                    }
                    break;
                }
            }

            if (!found) return;
        }
    }

    // ========== 列出內容 ==========

    // 列檔案內容
    if (!fileContents) return;
    for (const auto &fC : *fileContents) {
        std::cout << fC << std::endl;
    }

    parent->displayChse = 3;
}