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
#include <unordered_map>
#include <optional>
using namespace std;

extern string kbPrompt;
extern atomic<bool> escDetected;
extern atomic<bool> enterDetected;
extern atomic<bool> inputMasked;
extern atomic<bool> inputAte;
extern atomic<bool> kbEnabled;
extern atomic<bool> running;
extern atomic<bool> runningKb;
extern atomic<bool> runningMouse;
extern atomic<bool> mouseSync;
#ifdef __APPLE__
extern atomic<bool> isQuary;
extern atomic<int> cursorRow;
extern atomic<int> cursorCol;
#endif

struct Button {
    string name;
    int x, y, width, height;
};

class ManageInput {
public:
    ManageInput() : kb(this), mouse(this) {} // kb, mouse 綁定 this
    #ifdef _WIN32
    HANDLE hIn;
    #endif
    using Callback = function<void(const string&)>;
    struct Keyboard {
    public:
        Keyboard(ManageInput* p) : parent(p) {}
        #ifdef _WIN32
        void initial();                   // 啟動滑鼠 thread (Windows)
        void stop();                    // 停止滑鼠 thread
        #endif
        void disable() { kbEnabled = false; }    // 停用鍵盤輸入
        void enable()  { kbEnabled = true; }     // 啟用鍵盤輸入
        bool isEnabled() const { return kbEnabled; }
        string getInput();      // 取得輸入後的字串
        void spReset();
    private:
        ManageInput* parent;
        // 鍵盤/密碼
    };
    Keyboard kb;
    
    struct Mouse {
    public:
        Mouse(ManageInput* p) : parent(p) {}
        // 滑鼠功能
        #ifdef _WIN32
        void initial();                   // 啟動滑鼠 thread (Windows)
        void stop();                    // 停止滑鼠 thread
        #endif
        void btnAdd(const string& name, int x, int y, int w, int h);
        void btnDel(const vector<string>& names);
        void cbCreate(const string& name, Callback cb);          // 設定滑鼠 callback
        void cbClean(const optional<string>& name = nullopt);                      // 清理 callback
    private:
        ManageInput* parent;
    };
    Mouse mouse;

    #ifdef _WIN32
    void initial() {
        kb.initial();
        mouse.initial();
    }
    void stop() {
        kb.stop();
        mouse.stop();
    };
    #elif __APPLE__
    void initial();
    void stop();
    #endif

    // 同步等待
    void async(const int type) {
        if (type == 1) { // Mouse + Keyboard
            mouseSync = true;
            while (true) {
                if (!mouseSync) break;
                if (enterDetected || escDetected) {
                    mouseSync = false;
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        } else if (type == 11) { // Mouse + Keyboard (Disabled Esc)
            mouseSync = true;
            while (true) {
                if (!mouseSync) break;
                if (enterDetected) {
                    mouseSync = false;
                    break;
                }
                if (escDetected) escDetected = false;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        } else if (type == 2) { // Only Keyboard
            while (true) {
                if (enterDetected || escDetected) break;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        } else if (type == 21) { // Only keyboard (Disabled Esc)
            while (true) {
                if (enterDetected) break;
                if (escDetected) escDetected = false;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        } else if (type == 3) { // Only Mouse
            mouseSync = true;
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
private:
    #ifdef __APPLE__
    thread inputThread;
    #endif
    // Keyboard
    vector<int> blockedKeys;
    atomic<bool> blocking{false};
    #ifdef _WIN32
    thread kbThread;
    #endif
    atomic<bool> running{false};
    string lastInput;
    mutex inputMutex;
    // Mouse
    vector<Button> buttons;
    atomic<bool> runningMouse{false};
    #ifdef _WIN32
    thread mouseThread;
    #endif
    Callback currentCallback{nullptr};
    unordered_map<string, Callback> callbacks;
    mutex cbMutex;
    bool pointInButton(int px, int py, const Button& b) const {
        return px >= b.x && px < b.x + b.width &&
        py >= b.y && py < b.y + b.height;
    }
    // 在 ManageInput 類中新增成員變數：
    #ifdef _WIN32
    COORD startPos;
    size_t startCol = 0;
    size_t prevBufferLength = 0;
    string buffer;
    size_t cursorPos = 0;
    #else
    int startCol = 0;
    #endif
};