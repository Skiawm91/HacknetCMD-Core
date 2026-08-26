#include "os.h"
#include "HNCIP.h"
#include "console.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
using std::vector, std::string, std::cout, std::endl, std::stringstream;

extern Console con;

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

void hnfcOS::Command::Move(const string& src, const string& dst, HNCInterPreter::NodeInfo& node) {
    bool srcAbsolute = (!src.empty() && src[0] == '/');
    bool dstAbsolute = (!dst.empty() && dst[0] == '/');

    // 解析來源
    string srcName = src;
    string srcDir = "";
    size_t slash = src.rfind('/');
    if (slash != string::npos) {
        srcName = src.substr(slash + 1);
        srcDir = src.substr(0, slash);
    }

    vector<HNCInterPreter::NodeInfo::FolderEntry>* srcFolders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* srcFiles;
    bool srcPathValid = resolvePath(srcDir, srcAbsolute, parent->path, node, srcFolders, srcFiles);

    // 找來源檔案
    HNCInterPreter::NodeInfo::FileEntry* srcEntry = nullptr;
    if (srcPathValid) {
        for (auto& f : *srcFiles) {
            if (f.name == srcName) { srcEntry = &f; break; }
        }
    }

    // 預先檢查目標路徑
    vector<HNCInterPreter::NodeInfo::FolderEntry>* dstFolders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* dstFiles;
    resolvePath(dst, dstAbsolute, parent->path, node, dstFolders, dstFiles);

    string dstName = srcName;
    string dstLast = dst;
    size_t dslash = dst.rfind('/');
    
    // 如果結尾是 / 則整個 dst 是資料夾路徑
    bool dstEndsWithSlash = (!dst.empty() && dst.back() == '/');
    
    if (dslash != string::npos) {
        if (dstEndsWithSlash) dstLast = ""; // 整個是資料夾路徑
        else dstLast = dst.substr(dslash + 1);
    }

    bool dstIsFolder = dstEndsWithSlash;
    if (!dstIsFolder) {
        for (const auto& f : *dstFolders) {
            if (f.name == dstLast) { dstIsFolder = true; break; }
        }
    }

    bool dstDirValid = true;
    if (!dstIsFolder && dslash == string::npos) {
        // 純改名，目標在當前目錄
        dstName = dst;
        resolvePath("", false, parent->path, node, dstFolders, dstFiles);
    } else if (dstIsFolder) {
        // 整個 dst 是資料夾路徑
        dstDirValid = resolvePath(dst, dstAbsolute, parent->path, node, dstFolders, dstFiles);
    } else {
        // 有路徑且最後一段是新檔名
        string dstDirPart = dst.substr(0, dslash);
        dstName = dstLast;
        dstDirValid = resolvePath(dstDirPart, dstAbsolute, parent->path, node, dstFolders, dstFiles);
    }

    // 報錯
    if (!dstDirValid && !srcEntry) {
        con.println("Invalid Path").save();
        con.println("File not found!").save();
        return;
    }
    if (!dstDirValid) {
        con.println("Invalid Path").save();
        return;
    }
    if (!srcEntry) {
        con.println("File not found!").save();
        return;
    }

    // 複製到目標
    HNCInterPreter::NodeInfo::FileEntry newEntry = *srcEntry;
    newEntry.name = dstName;
    dstFiles->push_back(newEntry);

    // 從來源刪除
    for (auto it = srcFiles->begin(); it != srcFiles->end(); ++it) {
        if (it->name == srcName) {
            srcFiles->erase(it);
            break;
        }
    }

    parent->sys.saveNode(node);
}