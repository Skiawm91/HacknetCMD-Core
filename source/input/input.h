#pragma once
#include <vector>
#include <atomic>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
using namespace std;

extern atomic<bool> escDetected;
extern atomic<bool> runningKb;
extern atomic<bool> runningPwd;
extern atomic<bool> runningMouse;
extern atomic<bool> mouseSync;

struct Button {
    string name;
    int x, y, width, height;
};

class ManageInput {
public:
    // 鍵盤功能
    void keyDisable(const vector<int>& keys);
    void keyDisable(int key);
    void keyEnable();
    bool isBlocking() const;
    using Callback = function<void(const string&)>;

    void kbInput(const string& prompt, Callback cb, int exitCode);
    void stopKbInput();

    void pwdInput(const string &prompt, Callback cb, int exitCode);
    void stopPwdInput();

    // 滑鼠功能
    void mouseInput();                   // 啟動滑鼠 thread
    void btnAdd(const string& name, int x, int y, int w, int h);
    void btnDel(const vector<string>& names);
    void cbCreate(Callback cb);          // 設定滑鼠 callback
    void cbClean();                      // 清理 callback
    void stopMouse();                    // 停止滑鼠 thread

    // 停止全部
    void stopAll() {
        stopKbInput();
        stopPwdInput();
        stopMouse();
    };

private:
    // 鍵盤/密碼
    vector<int> blockedKeys;
    atomic<bool> blocking{false};
    void inputLoop();
    thread kbInputThread;
    thread pwdInputThread;

    // 滑鼠
    vector<Button> buttons;
    atomic<bool> runningMouse{false};
    thread mouseThread;
    Callback currentCallback{nullptr};
    mutex cbMutex;
    bool pointInButton(int px, int py, const Button& b) const {
        return px >= b.x && px < b.x + b.width &&
               py >= b.y && py < b.y + b.height;
    }

#ifdef _WIN32
    void kbWindowsInput(const string& prompt, Callback cb, int exitCode);
    void pwdWindowsInput(const string &prompt, Callback cb, int exitCode);
    void runMouseWindows();             // 滑鼠 thread
#else
    void kbMacInput(const string& prompt, Callback cb, int exitCode);
    void pwdMacInput(const string& prompt, Callback cb, int exitCode);
    void runMouseMac();                 // 滑鼠 thread
#endif
};
