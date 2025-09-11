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

atomic<bool> mouseSync;

void ManageInput::mouseInput() {
    if (runningMouse) return; // 避免重複啟動
    runningMouse = true;
    mouseThread = thread([this]() {
        bool pressed = false; // 控制一次點擊只觸發一次

        INPUT_RECORD ir;
        DWORD readCount;

        while (runningMouse) {
            // 只讀滑鼠事件，不干擾鍵盤
            if (ReadConsoleInput(hIn, &ir, 1, &readCount) && readCount == 1) {
                if (ir.EventType == MOUSE_EVENT) {
                    auto &me = ir.Event.MouseEvent;

                    if ((me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) && !pressed) {
                        pressed = true;
                        int x = me.dwMousePosition.X;
                        int y = me.dwMousePosition.Y;

                        for (auto &b : buttons) {
                            if (pointInButton(x, y, b)) {
                                lock_guard<mutex> lock(cbMutex);
                                if (currentCallback) currentCallback(b.name);
                                buffer.clear();
                                cursorPos = 0;
                                mouseSync = false;
                                break;
                            }
                        }
                    }

                    if (me.dwButtonState == 0) pressed = false;
                } else {
                    // ⚠️ 把不是滑鼠的事件丟回去，不要吃掉鍵盤
                    DWORD written;
                    WriteConsoleInput(hIn, &ir, 1, &written);
                }
            }
        }
    });
}

void ManageInput::stopMouse() {
    runningMouse = false;
    if (mouseThread.joinable()) mouseThread.join();
}

void ManageInput::btnAdd(const string& name, int x, int y, int w, int h) {
    buttons.push_back({name, x, y, w, h});
}

void ManageInput::btnDel(const vector<string>& names) {
    buttons.erase(
        remove_if(buttons.begin(), buttons.end(),
                  [&](const Button& b) {
                      return find(names.begin(), names.end(), b.name) != names.end();
                  }),
        buttons.end()
    );
}

void ManageInput::cbCreate(Callback cb) {
    lock_guard<mutex> lock(cbMutex);
    currentCallback = cb;
}

void ManageInput::cbClean() {
    lock_guard<mutex> lock(cbMutex);
    currentCallback = nullptr;
}
#endif