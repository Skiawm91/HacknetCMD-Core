#define _HAS_STD_BYTE 0
#include "input.h"
#include <conio.h>
#include <iostream>
#include <atomic>
#include <vector>
#include <mutex>
#include <windows.h>
using namespace std;

atomic<bool> escDetected;
atomic<bool> enterDetected;
atomic<bool> inputMasked;
atomic<bool> runningKb;
atomic<bool> kbEnabled;

void ManageInput::kbInput() {
    if (running) return;
    running = true;
    runningKb = true;

    kbThread = thread([this]() {
        string buffer;
        size_t cursorPos = 0;
        vector<string> history;
        int historyIndex = -1;

        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        // 初始化起始座標
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        startPos = csbi.dwCursorPosition;

        auto redrawLine = [&](size_t cursor) {
            SetConsoleCursorPosition(hOut, startPos);
            string display = inputMasked ? string(buffer.size(), '*') : buffer;
            cout << display << ' '; // 空格覆蓋尾巴
            // 移動游標到 cursor 位置
            COORD pos = startPos;
            pos.X += static_cast<SHORT>(cursor);
            SetConsoleCursorPosition(hOut, pos);
        };

        while (running) {
            if (!kbEnabled) {
                buffer.clear();
                cursorPos = 0;
                FlushConsoleInputBuffer(hIn);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            if (_kbhit()) {
                char c = _getch();

                if (c == 27) { // ESC
                    escDetected = true;
                } else if (c == '\r') { // Enter
                    {
                        lock_guard<mutex> lock(inputMutex);
                        lastInput = buffer;
                    }
                    if (!buffer.empty()) history.push_back(buffer);
                    historyIndex = history.size();
                    buffer.clear();
                    cursorPos = 0;
                    enterDetected = true;
                    cout << "\n";
                    GetConsoleScreenBufferInfo(hOut, &csbi);
                    startPos = csbi.dwCursorPosition; // 新行起始
                } else if (c == '\b') { // Backspace
                    if (cursorPos > 0) {
                        buffer.erase(cursorPos - 1, 1);
                        cursorPos--;
                        redrawLine(cursorPos);
                    }
                } else if (c == 0 || c == -32) { // 特殊鍵
                    char c2 = _getch();
                    if (c2 == 75) { // ←
                        if (cursorPos > 0) cursorPos--;
                        redrawLine(cursorPos);
                    } else if (c2 == 77) { // →
                        if (cursorPos < buffer.size()) cursorPos++;
                        redrawLine(cursorPos);
                    } else if (c2 == 72) { // ↑
                        if (!history.empty() && historyIndex > 0) {
                            historyIndex--;
                            buffer = history[historyIndex];
                            cursorPos = buffer.size();
                            redrawLine(cursorPos);
                        }
                    } else if (c2 == 80) { // ↓
                        if (!history.empty() && historyIndex < history.size() - 1) {
                            historyIndex++;
                            buffer = history[historyIndex];
                        } else {
                            buffer.clear();
                        }
                        cursorPos = buffer.size();
                        redrawLine(cursorPos);
                    }
                } else { // 一般字符
                    buffer.insert(cursorPos, 1, c);
                    cursorPos++;
                    redrawLine(cursorPos);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });
}

void ManageInput::spReset() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    startPos = csbi.dwCursorPosition;
}

void ManageInput::stopKb() {
    running = false;
    runningKb = false;
    if (kbThread.joinable()) kbThread.join();
}

string ManageInput::getInput() {
    lock_guard<mutex> lock(inputMutex);
    return lastInput;
}
