#define _HAS_STD_BYTE 0
#include "input.h"
#include <atomic>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <iostream>
#include <sstream>
#endif
#include <chrono>

atomic<bool> mouseSync;

void ManageInput::mouseInput() {
    if (runningMouse) return; // 避免重複啟動
    runningMouse = true;
    #ifdef _WIN32
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
#elif __APPLE__
    std::thread mouseThread([&]() {
        bool pressed = false;

        while (runningMouse) {
            // 取得滑鼠在螢幕上的位置
            CGEventRef event = CGEventCreate(nullptr);
            CGPoint mouseLoc = CGEventGetLocation(event);
            CFRelease(event);

            // 計算視窗左上角
            CGWindowID frontWinID = 0;
            CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
            if (windowList) {
                CFIndex count = CFArrayGetCount(windowList);
                for (CFIndex i = 0; i < count; i++) {
                    CFDictionaryRef winInfo = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
                    CFNumberRef layerNum = (CFNumberRef)CFDictionaryGetValue(winInfo, kCGWindowLayer);
                    int layer = 0;
                    CFNumberGetValue(layerNum, kCFNumberIntType, &layer);
                    if (layer == 0) { // 最前端普通應用視窗
                        CFNumberRef winIDNum = (CFNumberRef)CFDictionaryGetValue(winInfo, kCGWindowNumber);
                        CFNumberGetValue(winIDNum, kCFNumberIntType, &frontWinID);
                        break;
                    }
                }
                CFRelease(windowList);
            }

            CGRect winBounds = CGRectZero;
            if (frontWinID != 0) {
                CFArrayRef winInfoArr = CGWindowListCreateDescriptionFromArray(CFArrayCreate(nullptr, (const void**)&frontWinID, 1, nullptr));
                if (winInfoArr) {
                    CFDictionaryRef winInfo = (CFDictionaryRef)CFArrayGetValueAtIndex(winInfoArr, 0);
                    CFDictionaryRef boundsDict = (CFDictionaryRef)CFDictionaryGetValue(winInfo, kCGWindowBounds);
                    CGRectMakeWithDictionaryRepresentation(boundsDict, &winBounds);
                    CFRelease(winInfoArr);
                }
            }

            // 計算滑鼠相對視窗座標
            int relX = mouseLoc.x - winBounds.origin.x;
            int relY = mouseLoc.y - winBounds.origin.y;

            // 觸發左鍵
            CGEventFlags flags = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
            bool leftDown = (flags & kCGEventFlagMaskCommand) == 0; // 可改成檢查實際按鍵

            if (leftDown && !pressed) {
                pressed = true;
                for (auto &b : buttons) {
                    if (pointInButton(relX, relY, b)) {
                        std::lock_guard<std::mutex> lock(cbMutex);
                        if (currentCallback) currentCallback(b.name);
                        mouseSync = false;
                        break;
                    }
                }
            } else if (!leftDown) {
                pressed = false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    mouseThread.detach();
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