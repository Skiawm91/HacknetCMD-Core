#ifdef _WIN32
#define _HAS_STD_BYTE 0
#include "input.h"
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#include <windows.h>
#include <chrono>
#include <optional>

atomic<bool> mouseSync;

void ManageInput::Mouse::initial() {
    if (parent->runningMouse) return; // 避免重複啟動
    parent->runningMouse = true;
    parent->mouseThread = thread([this]() {
        bool pressed = false; // 控制一次點擊只觸發一次

        INPUT_RECORD ir;
        DWORD readCount;

        while (parent->runningMouse) {
            // 只讀滑鼠事件，不干擾鍵盤
            if (ReadConsoleInput(parent->hIn, &ir, 1, &readCount) && readCount == 1) {
                if (ir.EventType == MOUSE_EVENT) {
                    auto &me = ir.Event.MouseEvent;

                    if ((me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) && !pressed) {
                        pressed = true;
                        int x = me.dwMousePosition.X;
                        int y = me.dwMousePosition.Y;

                        for (auto &b : parent->buttons) {
                            if (parent->pointInButton(x, y, b)) {
                                lock_guard<mutex> lock(parent->cbMutex);
                                for (auto& [name, cb] : parent->callbacks) if (cb) cb(b.name);
                                parent->buffer.clear();
                                parent->cursorPos = 0;
                                mouseSync = false;
                                break;
                            }
                        }
                    }

                    if (me.dwButtonState == 0) pressed = false;
                } else {
                    // ⚠️ 把不是滑鼠的事件丟回去，不要吃掉鍵盤
                    DWORD written;
                    WriteConsoleInput(parent->hIn, &ir, 1, &written);
                }
            }
        }
    });
}

void ManageInput::Mouse::stop() {
    parent->runningMouse = false;
    if (parent->mouseThread.joinable()) parent->mouseThread.join();
}

void ManageInput::Mouse::btnAdd(const string& name, int x, int y, int w, int h) {
    parent->buttons.push_back({name, x, y, w, h});
}

void ManageInput::Mouse::btnDel(const vector<string>& names) {
    parent->buttons.erase(
        remove_if(parent->buttons.begin(), parent->buttons.end(),
                  [&](const Button& b) {
                      return find(names.begin(), names.end(), b.name) != names.end();
                  }),
        parent->buttons.end()
    );
}

void ManageInput::Mouse::cbCreate(const string& name, Callback cb) {
    lock_guard<mutex> lock(parent->cbMutex);
    parent->callbacks[name] = cb;
}

void ManageInput::Mouse::cbClean(const optional<string>& name) {
    lock_guard<mutex> lock(parent->cbMutex);
    if (name) parent->callbacks.erase(*name);
    else parent->callbacks.clear();
}
#endif