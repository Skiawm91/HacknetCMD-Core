#include "os.h"
#include "console.h"
#include <vector>
#include <string>
#include <iostream>
using std::vector, std::string;

extern Console con;

void hnfcOS::Command::Concatenate(const string &targetName, HNCInterPreter::NodeInfo& node) {
    string currentWorkingDir = parent->path.empty() ? "/" : parent->path;
    string fullPath;

    if (targetName.empty()) {
        return;
    } else if (targetName[0] == '/') {
        fullPath = targetName;
    } else {
        if (currentWorkingDir.back() != '/') {
            fullPath = currentWorkingDir + '/' + targetName;
        } else {
            fullPath = currentWorkingDir + targetName;
        }
    }

    vector<string> parts;
    size_t start = (fullPath.front() == '/') ? 1 : 0;
    size_t end;

    while ((end = fullPath.find('/', start)) != string::npos) {
        string part = fullPath.substr(start, end - start);
        if (part == "..") {
            if (!parts.empty()) {
                parts.pop_back();
            }
        } else if (part != "." && !part.empty()) {
            parts.push_back(part);
        }
        start = end + 1;
    }
    string lastPart = fullPath.substr(start);
    if (lastPart == "..") {
        if (!parts.empty()) parts.pop_back();
    } else if (lastPart != "." && !lastPart.empty()) {
        parts.push_back(lastPart);
    }
    
    if (parts.empty() && fullPath.front() == '/') {
        return;
    }

    vector<HNCInterPreter::NodeInfo::FolderEntry>* folderList = &node.folders;
    vector<HNCInterPreter::NodeInfo::FileEntry>* fileList = &node.files;
    string finalFileName = parts.back();

    for (size_t i = 0; i < parts.size() - 1; ++i) {
        const string& part = parts[i];
        bool found = false;

        for (auto& f : *folderList) {
            if (f.name == part) {
                folderList = &f.subfolders;
                fileList = &f.files;
                found = true;
                break;
            }
        }
        if (!found) {
            return;
        }
    }

    vector<string>* fileContents = nullptr;
    bool fileFound = false;

    for (auto &f : *fileList) {
        if (f.name == finalFileName) {
            fileContents = &f.contents;
            parent->file = &f;
            fileFound = true;
            break;
        }
    }

    vector<string> objectContents;

    if (!fileFound) {
        return;
    }

    for (const auto &fC : *fileContents) {
        if (parent->Mode == "Terminal") {
            con.println(fC).save();
        } else {
            objectContents.push_back(fC);
        }
    }

    if (parent->Mode == "Display") {
        parent->termTasks.push_back([objectContents]() {
            for (const auto& oC : objectContents) {
                con.println(oC).save();
            }
        });
    }

    parent->displayChse = 3;
}