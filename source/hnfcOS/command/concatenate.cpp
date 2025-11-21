#include "os.h"
#include <vector>
#include <string>
#include <iostream>
using std::vector, std::string;

void hnfcOS::Command::Concatenate(const string &targetName, HNCInterPreter::NodeInfo& node) {
    string currentPath = parent->path.empty() ? "/" : parent->path;

    // 1. 找到目標檔案所在的資料夾
    vector<HNCInterPreter::NodeInfo::FolderEntry>* folderList = &node.folders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* fileList = &node.files;

    if (currentPath != "/") {
        vector<string> parts;
        size_t start = 1, end;

        // 解析 currentPath 成 parts (例如：/a/b/ -> ["a", "b"])
        while ((end = currentPath.find('/', start)) != string::npos) {
            parts.push_back(currentPath.substr(start, end - start));
            start = end + 1;
        }

        // 遍歷所有資料夾層級
        for (const auto& part : parts) {
            bool found = false;

            for (auto& f : *folderList) {
                if (f.name == part) {
                    // 更新到下一層
                    folderList = &f.subfolders;
                    fileList = &f.files;
                    found = true;
                    break;
                }
            }
            // 如果中間路徑上的資料夾不存在，則退出
            if (!found) return;
        }
    }

    // 2. 在最終的 fileList 中查找目標檔案 (targetName)
    vector<string>* fileContents = nullptr;
    bool fileFound = false;

    for (auto &f : *fileList) {
        if (f.name == targetName) {
            fileContents = &f.contents;
            f.opened = true; // 設置開啟狀態
            parent->file = &f;
            fileFound = true;
            break;
        }
    }

    // 3. 處理結果並列印內容


    vector<string> objectContents;

    if (!fileFound) {
        return;
    }

    // 列印檔案內容
    for (const auto &fC : *fileContents) {
        if (parent->Mode == "Terminal") {
            std::cout << fC << std::endl;
        } else {
            objectContents.push_back(fC);
        }
    }

    if (parent->Mode == "Display") {
        parent->termTasks.push_back([objectContents]() {
            for (const auto& oC : objectContents) {
                std::cout << oC << std::endl;
            }
        });
    }

    parent->displayChse = 3;
}