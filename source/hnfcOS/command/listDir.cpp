#include "os.h"
#include <vector>
#include <string>
#include <iostream>
using std::vector, std::string;

void hnfcOS::Command::ListDir(HNCInterPreter::NodeInfo& node) {
    string currentPath = parent->path.empty() ? "/" : parent->path;

    // 判斷目前 folder（root = nullptr）
    HNCInterPreter::NodeInfo::FolderEntry* currentFolder = nullptr;
    vector<HNCInterPreter::NodeInfo::FolderEntry>* folderList = &node.folders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* fileList = &node.files;

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
                    found = true;
                    break;
                }
            }

            if (!found) {
                if (parent->Mode == "Terminal") {
                    std::cout << "Path error." << std::endl;
                }
                return;
            }
        }
    }

    // ========== 列出內容 ==========
    vector<string> objectNames;

    // 列資料夾
    for (const auto& sf : *folderList) {
        if (parent->Mode == "Terminal") {
            std::cout << ":" << sf.name << std::endl;
        } else {
            objectNames.push_back(":" + sf.name);
        }
    }

    // 列檔案
    for (const auto& f : *fileList) {
        if (parent->Mode == "Terminal") {
            std::cout << f.name << std::endl;
        } else {
            objectNames.push_back(f.name);
        }
    }

    // Display 模式：排入 termTasks
    if (parent->Mode == "Display") {
        parent->termTasks.push_back([objectNames]() {
            for (const auto& oN : objectNames) {
                std::cout << oN << std::endl;
            }
        });
    }

    parent->displayChse = 3;
}