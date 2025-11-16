#include "os.h"
#include <string>
using std::string, std::vector;

void hnfcOS::Command::ChangeDir(HNCInterPreter::NodeInfo& node, const string& dir) {
    if (dir.empty()) return;

    // 將輸入路徑拆成資料夾名稱
    vector<string> foldersToCd;
    size_t start = 0, end;
    while ((end = dir.find('/', start)) != string::npos) {
        string part = dir.substr(start, end - start);
        if (!part.empty()) foldersToCd.push_back(part);
        start = end + 1;
    }
    if (start < dir.size())
        foldersToCd.push_back(dir.substr(start));

    vector<HNCInterPreter::NodeInfo::FolderEntry>* folders = &node.folders;
    string newPath = "/";
    vector<HNCInterPreter::NodeInfo::FolderEntry*> stack; // 追蹤展開層級

    for (auto &folderName : foldersToCd) {
        if (folderName == ".") {
            return;
        } else if (folderName == "..") {
            if (!stack.empty()) {
                auto *last = stack.back();
                last->expand = 0; // 收起上一層
                stack.pop_back();
                // 更新 newPath
                if (newPath.back() == '/') newPath.pop_back();
                size_t pos = newPath.rfind('/');
                newPath = (pos != string::npos) ? newPath.substr(0, pos+1) : "/";
                folders = (stack.empty()) ? &node.folders : &stack.back()->subfolders;
            }
            break;
        }

        bool found = false;
        for (auto &f : *folders) {
            if (f.name == folderName) {
                // 收起同層其他資料夾
                for (auto &sibling : *folders)
                    if (&sibling != &f) sibling.expand = 0;

                f.expand = 2;              // 標記已展開
                stack.push_back(&f);       // 推入堆疊
                newPath += f.name + "/";   // 更新新路徑
                folders = &f.subfolders;   // 進入下一層
                found = true;
                break;
            }
        }
        if (!found) {
            // 找不到資料夾就退出
            return;
        }
    }

    parent->path = newPath;
    parent->displayChse = 3;
}
