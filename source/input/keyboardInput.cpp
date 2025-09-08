#define _HAS_STD_BYTE 0
#include "input.h"
#include <iostream>
#include <atomic>
#include <vector>
#include <mutex>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif
using namespace std;

atomic<bool> escDetected;
atomic<bool> enterDetected;
atomic<bool> inputMasked;
atomic<bool> runningKb;
atomic<bool> kbEnabled;

#ifndef _WIN32
int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, nullptr, nullptr, &tv);
}

char getch() {
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    return c;
}
#endif

void ManageInput::kbInput() {
    if (running) return;
    running = true;
    runningKb = true;

    #ifdef _WIN32
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
    #else
    kbThread = thread([this]() {
        string buffer;
        size_t cursorPos = 0;
        vector<string> history;
        int historyIndex = -1;

        if (startCol < 0) startCol = 0;

        auto redrawLine = [&](size_t cursor) {
            // 移動到行首
            cout << "\r";
            // 輸出完整行
            string display = inputMasked ? string(buffer.size(), '*') : buffer;
            cout << display;
            // 如果字串變短，用空格覆蓋殘留
            cout << string(max(0, static_cast<int>(buffer.size() - cursor)), ' ');
            // 移回游標
            cout << "\r\33[" << cursor + startCol << "C";
            cout.flush();
        };

        while (running) {
            if (!kbEnabled) {
                std::this_thread::sleep_for(5ms);
                continue;
            }

            if (kbhit()) {
                char c = getch();

                if (c == 27) { // ESC 或特殊序列
                    escDetected = true;
                    if (kbhit() && getch() == '[') {
                        char seq = getch();
                        if (seq == 'A' && historyIndex > 0) {
                            historyIndex--;
                            buffer = history[historyIndex];
                            cursorPos = buffer.size();
                        } else if (seq == 'B') {
                            if (!history.empty() && historyIndex < history.size() - 1) {
                                historyIndex++;
                                buffer = history[historyIndex];
                            } else {
                                buffer.clear();
                                historyIndex = history.size();
                            }
                            cursorPos = buffer.size();
                        } else if (seq == 'C' && cursorPos < buffer.size()) cursorPos++;
                        else if (seq == 'D' && cursorPos > 0) cursorPos--;
                        redrawLine(cursorPos);
                    }
                } else if (c == '\n' || c == '\r') { // Enter
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
                    startCol = 0;
                } else if (c == 127 || c == '\b') { // Backspace
                    if (cursorPos > 0) {
                        buffer.erase(cursorPos - 1, 1);
                        cursorPos--;
                        redrawLine(cursorPos);
                    }
                } else { // 一般字元
                    buffer.insert(cursorPos, 1, c);
                    cursorPos++;
                    redrawLine(cursorPos);
                }
            }

            std::this_thread::sleep_for(5ms);
        }
    });
    #endif
}

void ManageInput::spReset() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    startPos = csbi.dwCursorPosition;
    #else
    startCol = 0;
    #endif
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
