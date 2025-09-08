#ifdef __APPLE__

#include "input.h"
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>

atomic<bool> escDetected;
atomic<bool> enterDetected;
atomic<bool> inputMasked;
atomic<bool> kbEnabled;
atomic<bool> running;
atomic<bool> mouseSync;

void ManageInput::spReset() {
    startCol = 0;
}

void ManageInput::input() {
    if (running) return;
    running = true;

    // 啟用 VT100 滑鼠報告
    cout << "\033[?1000h";
    cout.flush();

    thread([this]() {
        string buffer;
        size_t cursorPos = 0;
        size_t inputStartPos = 0;     // 輸入開始位置
        vector<string> history;
        int historyIndex = -1;

        if (startCol < 0) startCol = 0;

        char buf[32];

        while (running) {
            int n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n <= 0) continue;

            for (int i = 0; i < n; ++i) {
                char c = buf[i];

                // 滑鼠事件
                if (c == '\033' && i + 5 < n && buf[i + 1] == '[' && buf[i + 2] == 'M') {
                    unsigned char cb = buf[i + 3];
                    unsigned char cx = buf[i + 4];
                    unsigned char cy = buf[i + 5];
                    int btn = cb - 32;
                    int col = cx - 32 - 1;
                    int row = cy - 32 - 1;

                    if ((btn & 0x03) == 0 && mouseSync) { // 左鍵
                        for (auto &b : buttons) {
                            if (pointInButton(col, row, b)) {
                                lock_guard<mutex> lock(cbMutex);
                                if (currentCallback) currentCallback(b.name);
                                mouseSync = false;
                            }
                        }
                    }
                    i += 5;
                    continue;
                }

                if (!kbEnabled) continue;

                // ESC / 方向鍵
                if (c == 27) {
                    escDetected = true;
                    char seq;
                    if (read(STDIN_FILENO, &seq, 1) > 0 && seq == '[') {
                        if (read(STDIN_FILENO, &seq, 1) > 0) {
                            if (seq == 'A' && historyIndex > 0) { // ↑
                                historyIndex--;
                                buffer = history[historyIndex];
                                cursorPos = buffer.size();
                            } else if (seq == 'B') { // ↓
                                if (!history.empty() && historyIndex < (int)history.size() - 1) {
                                    historyIndex++;
                                    buffer = history[historyIndex];
                                } else {
                                    buffer.clear();
                                    historyIndex = history.size();
                                }
                                cursorPos = buffer.size();
                            } else if (seq == 'C' && cursorPos < buffer.size()) cursorPos++; // →
                            else if (seq == 'D' && cursorPos > 0) cursorPos--;                 // ←
                        }
                    }
                }
                // Enter
                else if (c == '\n' || c == '\r') {
                    {
                        lock_guard<mutex> lock(inputMutex);
                        lastInput = buffer;
                    }
                    if (!buffer.empty()) history.push_back(buffer);
                    historyIndex = history.size();
                    buffer.clear();
                    cursorPos = 0;
                    inputStartPos = 0; // 重置輸入起始位置
                    enterDetected = true;
                    cout << "\n";
                }
                // Backspace
                else if (c == 127 || c == '\b') {
                    if (cursorPos > inputStartPos) {
                        buffer.erase(cursorPos - 1, 1);
                        cursorPos--;
                        cout << "\b \b";
                        cout.flush();
                    }
                }
                // 一般字符
                else {
                    buffer.insert(cursorPos, 1, c);
                    cursorPos++;
                    cout << c;
                    cout.flush();
                }
            }

            this_thread::sleep_for(chrono::milliseconds(5));
        }

        // 停用 VT100 滑鼠報告
        cout << "\033[?1000l";
        cout.flush();
    }).detach();
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

string ManageInput::getInput() {
    lock_guard<mutex> lock(inputMutex);
    return lastInput;
}

#endif
