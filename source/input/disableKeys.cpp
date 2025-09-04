// Disable keys by GPT
#define _HAS_STD_BYTE 0
#include "input.h"
#include <thread>
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#endif

void ManageInput::keyDisable(const std::vector<int>& keys) {
    blockedKeys = keys;
    blocking = true;
    std::thread(&ManageInput::inputLoop, this).detach();
}

void ManageInput::keyDisable(int key) {
    keyDisable(std::vector<int>{key});
}

// ✅ 新增：無參數版本 → 鎖所有
void ManageInput::keyDisable() {
    blockedKeys.clear();   // 清空，代表「所有按鍵都被阻止」
    blocking = true;
    std::thread(&ManageInput::inputLoop, this).detach();
}

void ManageInput::keyEnable() {
    blocking = false;
}

bool ManageInput::isBlocking() const {
    return blocking;
}

#ifdef __APPLE__ // macOS or Unix-like
void setNonBlockingInput(bool enable) {
    static struct termios oldt, newt;
    static bool configured = false;

    if (enable && !configured) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        configured = true;
    } else if (!enable && configured) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, 0);
        configured = false;
    }
}
#endif

void ManageInput::inputLoop() {
#ifdef _WIN32
    while (blocking) {
        if (_kbhit()) {
            int key = _getch();

            // 如果 blockedKeys 為空 → 全部阻止
            if (blockedKeys.empty()) {
                continue;
            }

            for (int blocked : blockedKeys) {
                if (key == blocked) {
                    // 吃掉，不輸出
                    break;
                }
            }
        }
        Sleep(10);
    }
#else
    setNonBlockingInput(true);
    while (blocking) {
        fd_set set;
        struct timeval timeout = {0, 10000}; // 10ms
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        if (select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout) > 0) {
            char ch;
            if (read(STDIN_FILENO, &ch, 1) > 0) {
                // 如果 blockedKeys 為空 → 全部阻止
                if (blockedKeys.empty()) {
                    continue;
                }

                for (int blocked : blockedKeys) {
                    if ((int)ch == blocked) {
                        // 吃掉，不輸出
                        break;
                    }
                }
            }
        }
    }
    setNonBlockingInput(false);
#endif
}