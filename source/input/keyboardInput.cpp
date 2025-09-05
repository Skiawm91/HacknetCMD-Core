#include "input.h"
#include <conio.h>
#include <iostream>
#include <atomic>
using namespace std;

atomic<bool> escDetected;
atomic<bool> enterDetected;
atomic<bool> inputMasked;
atomic<bool> runningKb;

void ManageInput::kbInput() {
    if (running) return; // 避免重複啟動
    running = true;
    runningKb = true;

    kbThread = thread([this]() {
        string buffer;
        while (running) {
            if (_kbhit()) {
                char c = _getch();

                if (c == 27) {          // ESC
                    escDetected = true;
                } else if (c == '\r') { // Enter -> 完成一次輸入
                    {
                        lock_guard<mutex> lock(inputMutex);
                        lastInput = buffer;
                    }
                    buffer.clear();
                    enterDetected = true;
                    cout << "\n"; // 顯示下一行 prompt
                } else if (c == '\b') { // Backspace
                    if (!buffer.empty()) {
                        buffer.pop_back();
                        cout << "\b \b";
                    }
                } else {
                    if (inputMasked) {
                        buffer.push_back(c);
                        cout << "*";
                    } else {
                        buffer.push_back(c);
                        cout << c;
                    }
                }
            }
        }
    });
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
