#define _HAS_STD_BYTE 0
#include "input.h"
#include <windows.h>
#include <algorithm>
#include <chrono>

atomic<bool> mouseSync{false};

void ManageInput::mouseInput() {
#ifdef _WIN32
    if (runningMouse) return; // 避免重複啟動

    runningMouse = true;
    mouseThread = thread([this]() {
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        DWORD prevMode;
        GetConsoleMode(hIn, &prevMode);

        // 停用 Quick Edit，啟用滑鼠事件
        DWORD mode = prevMode & ~ENABLE_QUICK_EDIT_MODE;
        mode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
        SetConsoleMode(hIn, mode);

        INPUT_RECORD ir;
        DWORD readCount;
        bool pressed = false; // 控制一次點擊只觸發一次

        while (runningMouse) {
            if (ReadConsoleInput(hIn, &ir, 1, &readCount) && readCount == 1) {
                if (ir.EventType == MOUSE_EVENT) {
                    auto &me = ir.Event.MouseEvent;

                    // 左鍵按下且之前沒按下
                    if ((me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) && !pressed) {
                        pressed = true;
                        int x = me.dwMousePosition.X;
                        int y = me.dwMousePosition.Y;

                        for (auto &b : buttons) {
                            if (pointInButton(x, y, b)) {
                                {
                                    lock_guard<mutex> lock(cbMutex);
                                    if (currentCallback) currentCallback(b.name);
                                }
                                mouseSync = false; // 控制外部 loop
                                break;
                            }
                        }
                    }

                    // 左鍵放開時重置 pressed
                    if (me.dwButtonState == 0) pressed = false;
                }
            } else {
                this_thread::sleep_for(chrono::milliseconds(5));
            }
        }

        // 恢復原本 console mode
        SetConsoleMode(hIn, prevMode);
    });
#endif
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