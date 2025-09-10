#define _HAS_STD_BYTE 0
#pragma once
#ifdef _WIN32
#include <windows.h>
#else
#endif
#include <vector>
#include <atomic>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
using namespace std;

extern string kbPrompt;
extern atomic<bool> escDetected;
extern atomic<bool> enterDetected;
extern atomic<bool> inputMasked;
extern atomic<bool> kbEnabled;
extern atomic<bool> running;
#ifdef _WIN32
extern atomic<bool> runningKb;
extern atomic<bool> runningMouse;
#endif
extern atomic<bool> mouseSync;

struct Button {
    string name;
    int x, y, width, height;
};

class ManageInput {
public:
    #ifdef _WIN32
    HANDLE hIn;
    #endif

    // 鍵盤功能
    void kbDisable() { kbEnabled = false; }    // 停用鍵盤輸入
    void kbEnable()  { kbEnabled = true; }     // 啟用鍵盤輸入
    bool isKbEnabled() const { return kbEnabled; }

    using Callback = function<void(const string&)>;

    #ifdef _WIN32
    void kbInput();         // 啟動鍵盤監聽 (Windows)
    void stopKb();          // 停止鍵盤監聽 (Windows)
    #endif
    string getInput();      // 取得輸入後的字串

    void spReset();

    // 滑鼠功能
    #ifdef _WIN32
    void mouseInput();                   // 啟動滑鼠 thread (Windows)
    void stopMouse();                    // 停止滑鼠 thread
    #endif
    void btnAdd(const string& name, int x, int y, int w, int h);
    void btnDel(const vector<string>& names);
    void cbCreate(Callback cb);          // 設定滑鼠 callback
    void cbClean();                      // 清理 callback

    // macOS / 整合版輸入
    #ifdef __APPLE__
    void input(); // 結合鍵盤 + VT100 滑鼠，支持 kbEnable/kbDisable & mouseSync
    void stopInput();
    #endif
    // 同步等待
    void async(const int type) {
        if (type == 1) {
            while (true) {
                if (!mouseSync) break;
                if (enterDetected || escDetected) {
                    mouseSync = false;
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        } else if (type == 2) {
            while (true) {
                if (enterDetected || escDetected) break;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        } else if (type == 3) {
            while (true) {
                if (!mouseSync) break;
                if (escDetected) {
                    escDetected = false;
                } else if (enterDetected) {
                    enterDetected = false;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }

    #ifdef _WIN32
    void stopAll() {
        stopKb();
        stopMouse();
    };
    #endif

private:
    // 鍵盤/密碼
    vector<int> blockedKeys;
    atomic<bool> blocking{false};
    thread kbThread;
    atomic<bool> running{false};
    string lastInput;
    mutex inputMutex;
    thread pwdInputThread;

    // 在 ManageInput 類中新增成員變數：
    #ifdef _WIN32
    COORD startPos;
    #else
    int startCol = 0;
    #endif

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
};