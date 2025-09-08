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
        vector<string> history;
        int historyIndex = -1;

        if (startCol < 0) startCol = 0;

        // 記錄行首位置（第一次繪製時取得）
        int startRow = 0;
        bool startRowSet = false;

        auto redrawLine = [&]() {
            // 設定游標回行首+起始列
            cout << "\033[s"; // 存游標
            cout << "\033[u"; // 還原游標
            if (!startRowSet) {
                cout << "\033[6n"; // 查詢游標位置
                char resp[32];
                int n = read(STDIN_FILENO, resp, sizeof(resp));
                if (n > 0) {
                    // ANSI 回傳格式：ESC[row;colR
                    int r=0,c=0;
                    if (sscanf(resp,"\033[%d;%dR",&r,&c)==2) {
                        startRow = r;
                        startRowSet = true;
                    }
                }
            }

            cout << "\033[" << startRow << ";" << (startCol+1) << "H"; // 移到起始位置

            string display = inputMasked ? string(buffer.size(), '*') : buffer;
            cout << display;
            // 移動游標到正確位置
            cout << "\033[" << (cursorPos+startCol+1) << "G";
            cout.flush();
        };

        char buf[32];
        while (running) {
            int n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n <= 0) {
                this_thread::sleep_for(chrono::milliseconds(5));
                continue;
            }

            for (int i=0;i<n;i++) {
                char c = buf[i];

                // 處理滑鼠事件
                if (c=='\033' && i+5<n && buf[i+1]=='[' && buf[i+2]=='M') {
                    unsigned char cb = buf[i+3];
                    unsigned char cx = buf[i+4];
                    unsigned char cy = buf[i+5];
                    int btn = cb-32;
                    int col = cx-32-1;
                    int row = cy-32-1;

                    if ((btn & 0x03)==0 && mouseSync) { // 左鍵
                        for (auto &b : buttons) {
                            if (pointInButton(col,row,b)) {
                                lock_guard<mutex> lock(cbMutex);
                                if (currentCallback) currentCallback(b.name);
                                mouseSync = false;
                                redrawLine(); // 滑鼠點擊也即時刷新
                            }
                        }
                    }
                    i+=5;
                    continue;
                }

                if (!kbEnabled) continue;

                // ESC / 方向鍵
                if (c==27) {
                    escDetected = true;
                    char seq;
                    if (read(STDIN_FILENO,&seq,1)>0 && seq=='[') {
                        if (read(STDIN_FILENO,&seq,1)>0) {
                            if (seq=='A' && historyIndex>0) { // ↑
                                historyIndex--;
                                buffer = history[historyIndex];
                                cursorPos = buffer.size();
                            } else if (seq=='B') { // ↓
                                if (!history.empty() && historyIndex<(int)history.size()-1) {
                                    historyIndex++;
                                    buffer = history[historyIndex];
                                } else {
                                    buffer.clear();
                                    historyIndex = history.size();
                                }
                                cursorPos = buffer.size();
                            } else if (seq=='C' && cursorPos<buffer.size()) cursorPos++; // →
                            else if (seq=='D' && cursorPos>0) cursorPos--; // ←
                            redrawLine();
                        }
                    }
                }
                else if (c=='\n' || c=='\r') {
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
                }
                else if (c==127 || c=='\b') { // Backspace
                    if (cursorPos>0) {
                        buffer.erase(cursorPos-1,1);
                        cursorPos--;
                        redrawLine();
                    }
                }
                else { // 一般字元
                    buffer.insert(cursorPos,1,c);
                    cursorPos++;
                    redrawLine();
                }
            }

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
