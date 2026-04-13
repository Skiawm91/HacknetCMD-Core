#include "os.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
using std::vector, std::string, std::cout, std::endl, std::stringstream;

void hnfcOS::Command::Remove(const string& path, HNCInterPreter::NodeInfo& node) {
    vector<HNCInterPreter::NodeInfo::FolderEntry>* targetFolders = &node.folders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* targetFiles = &node.files;

    // 先走到 parent->path 所在層
    if (!parent->path.empty() && parent->path != "/") {
        string p = parent->path;
        if (p.back() == '/') p.pop_back();
        stringstream ss(p);
        string part;
        while (getline(ss, part, '/')) {
            if (part.empty()) continue;
            for (auto& f : *targetFolders) {
                if (f.name == part) {
                    targetFiles = &f.files;
                    targetFolders = &f.subfolders;
                    break;
                }
            }
        }
    }

    // 切割 path，處理 ../
    vector<string> parts;
    stringstream ss(path);
    string part;
    while (getline(ss, part, '/')) {
        if (part.empty() || part == ".") continue;
        parts.push_back(part);
    }

    // 走到目標的父層（最後一個 part 是目標檔名）
    string fileName = parts.back();
    parts.pop_back();

    // 重建完整路徑stack 以支援 ..
    // 先把當前路徑拆成 stack
    vector<HNCInterPreter::NodeInfo::FolderEntry*> folderStack;
    if (!parent->path.empty() && parent->path != "/") {
        string p = parent->path;
        if (p.back() == '/') p.pop_back();
        stringstream ss2(p);
        vector<HNCInterPreter::NodeInfo::FolderEntry>* cur = &node.folders;
        while (getline(ss2, part, '/')) {
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

    // 處理 parts（含 ..）
    for (const auto& p : parts) {
        if (p == "..") {
            if (!folderStack.empty()) folderStack.pop_back();
        } else {
            vector<HNCInterPreter::NodeInfo::FolderEntry>* cur = folderStack.empty() ? &node.folders : &folderStack.back()->subfolders;
            for (auto& f : *cur) {
                if (f.name == p) {
                    folderStack.push_back(&f);
                    break;
                }
            }
        }
    }

    // 取得最終的 files
    if (folderStack.empty()) {
        targetFiles = &node.files;
    } else {
        targetFiles = &folderStack.back()->files;
    }

    // * 清空
    if (fileName == "*") {
        targetFiles->clear();
        parent->kit.saveNode(node);
        return;
    }

    // 刪除指定檔案
    for (auto it = targetFiles->begin(); it != targetFiles->end(); ++it) {
        if (it->name == fileName) {
            targetFiles->erase(it);
            parent->kit.saveNode(node);
            return;
        }
    }

    cout << "File " << path << " not found!" << endl;
}