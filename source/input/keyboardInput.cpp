#ifdef _WIN32
#define _HAS_STD_BYTE 0
#include "input.h"
#include <iostream>
#include <atomic>
#include <vector>
#include <mutex>
#include <conio.h>
#include <windows.h>
#include <string>
#include <codecvt>
using namespace std;

string kbPrompt;
atomic<bool> promptPrinted, escDetected, enterDetected, inputMasked, inputAte, runningKb, kbEnabled;

size_t utf8_width(const string &s) {
    size_t w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = s[i];
        if ((c & 0x80) == 0) {          // ASCII
            w += 1;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) { // 2-byte
            w += 2;
            i += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3-byte
            w += 2;
            i += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4-byte
            w += 2;
            i += 4;
        } else {
            // 避免死循環，跳過未知 byte
            i += 1;
        }
    }
    return w;
}

void ManageInput::Keyboard::initial() {
    if (parent->running) return;
    parent->running = true;
    runningKb = true;
        parent->kbThread = thread([this]() {

        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        // 初始化起始座標
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        parent->startPos = csbi.dwCursorPosition;

        auto redrawLine = [&](size_t cursor) {
            if (inputAte) return;
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD pos = parent->startPos;
            pos.X += static_cast<SHORT>(parent->startCol);
            SetConsoleCursorPosition(hOut, pos);
            string display = inputMasked ? string(parent->buffer.size(), '*') : parent->buffer;
            cout << display;
            if (parent->prevBufferLength > parent->buffer.size()) {
                cout << string(parent->prevBufferLength - parent->buffer.size(), ' ');
            }
            pos.X = static_cast<SHORT>(parent->startCol + cursor + parent->startPos.X);
            SetConsoleCursorPosition(hOut, pos);
            cout.flush();
            parent->prevBufferLength = parent->buffer.size();
        };

        while (parent->running) {
            if (!kbEnabled) {
                parent->buffer.clear();
                parent->cursorPos = 0;
                FlushConsoleInputBuffer(hIn);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            if (!promptPrinted) {
                cout << kbPrompt << flush;
                promptPrinted = true;
            }
            if (_kbhit()) {
                char c = _getch();

                if (c == 27) { // ESC
                    escDetected = true;
                } else if (c == '\r') { // Enter
                    {
                        lock_guard<mutex> lock(parent->inputMutex);
                        parent->lastInput = parent->buffer;
                    }
                    if (!parent->buffer.empty()) parent->history.push_back(parent->buffer);
                    parent->historyIndex = parent->history.size();
                    parent->buffer.clear();
                    parent->cursorPos = 0;
                    enterDetected = true;
                    cout << "\n";
                    GetConsoleScreenBufferInfo(hOut, &csbi);
                    parent->startPos = csbi.dwCursorPosition; // 新行起始
                } else if (c == '\b') { // Backspace
                    if (parent->cursorPos > 0) {
                        parent->buffer.erase(parent->cursorPos - 1, 1);
                        parent->cursorPos--;
                        redrawLine(parent->cursorPos);
                    }
                } else if (c == 0 || c == -32) { // 特殊鍵
                    char c2 = _getch();
                    if (c2 == 75) { // ←
                        if (parent->cursorPos > 0) parent->cursorPos--;
                        redrawLine(parent->cursorPos);
                    } else if (c2 == 77) { // →
                        if (parent->cursorPos < parent->buffer.size()) parent->cursorPos++;
                        redrawLine(parent->cursorPos);
                    } else if (c2 == 72) { // ↑
                        if (!parent->history.empty() && parent->historyIndex > 0) {
                            parent->historyIndex--;
                            parent->buffer = parent->history[parent->historyIndex];
                            parent->cursorPos = parent->buffer.size();
                            redrawLine(parent->cursorPos);
                        }
                    } else if (c2 == 80) { // ↓
                        if (!parent->history.empty() && parent->historyIndex < parent->history.size() - 1) {
                            parent->historyIndex++;
                            parent->buffer = parent->history[parent->historyIndex];
                        } else {
                            parent->buffer.clear();
                        }
                        parent->cursorPos = parent->buffer.size();
                        redrawLine(parent->cursorPos);
                    }
                } else { // 一般字符
                    parent->buffer.insert(parent->cursorPos, 1, c);
                    parent->cursorPos++;
                    redrawLine(parent->cursorPos);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });
}

void ManageInput::Keyboard::spReset() {
    promptPrinted = false;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    parent->startPos = csbi.dwCursorPosition;
    parent->startCol = static_cast<int>(utf8_width(kbPrompt));
    parent->prevBufferLength = 0;
}

void ManageInput::Keyboard::stop() {
    parent->running = false;
    runningKb = false;
    if (parent->kbThread.joinable()) parent->kbThread.join();
}

void ManageInput::Keyboard::historyClear() {
    parent->history.clear();
    parent->historyIndex = parent->history.size();
}

string ManageInput::Keyboard::getInput() {
    lock_guard<mutex> lock(parent->inputMutex);
    return parent->lastInput;
}
#endif