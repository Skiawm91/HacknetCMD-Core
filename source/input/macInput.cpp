#ifdef __APPLE__
#include "input.h"
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <chrono>
using namespace std;

string kbPrompt;
atomic<bool> promptPrinted, escDetected, enterDetected, inputMasked, kbEnabled, mouseSync;

static int read_with_timeout(int fd, char *buf, int maxlen, int timeout_ms) {
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    int rv = select(fd + 1, &rfds, NULL, NULL, &tv);
    if (rv > 0) {
        int r = read(fd, buf, maxlen);
        if (r < 0 && errno == EAGAIN) return 0;
        return r;
    }
    return 0;
}

void ManageInput::spReset() {
    // macOS: caller 控制 startCol（通常呼叫者知道 prompt 寬度）
    // 這裡我們把起始 column 設為 0（若需要可改為取得實際游標）
    promptPrinted = false;
    startCol = kbPrompt.size();
}

// single combined input loop for macOS
void ManageInput::input() {
    if (running) return;
    running = true;

    kbThread = thread([this]() {
        string buffer;
        size_t cursorPos = 0;
        vector<string> history;
        int historyIndex = -1;
        bool cbDone = false;

        // 確保 startCol 有合理值
        if (startCol < 0) startCol = 0;

        auto redrawAfterPrompt = [&](size_t cursor) {
            // 不清整行，只覆寫 prompt 後面（從 startCol 開始）
            // 移到行首然後移到 startCol
            cout << "\r";
            if (startCol > 0) cout << "\033[" << startCol << "C";
            // 顯示 masked 或 raw buffer
            string display = inputMasked ? string(buffer.size(), '*') : buffer;
            cout << display;
            // 覆蓋可能遺留的尾巴
            cout << "\033[K";
            // 把游標移回到 cursor 位置
            cout << "\r";
            if (startCol > 0) cout << "\033[" << startCol << "C";
            if (cursor > 0) cout << "\033[" << cursor << "C";
            cout.flush();
        };

        // local temporary buffer for raw read
        char rbuf[128];

        while (running) {
            // 若 kbEnabled 且 prompt 尚未印過，印一次（不在每回合重印）
            if (kbEnabled && !promptPrinted) {
                cout << kbPrompt << flush;
                // prompt 已在外面印或由這裡印， startCol 應該是 prompt 寬度
                // 若你要精確計算，請呼叫 spReset 來更新 startCol
                promptPrinted = true;
            }

            // 直接用 blocking read (但短 timeout) 來一次拿多個 bytes，之後逐個解析
            int n = read_with_timeout(STDIN_FILENO, rbuf, (int)sizeof(rbuf), 50); // 50ms
            if (n <= 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
                continue;
            }

            int i = 0;
            while (i < n) {
                unsigned char c = static_cast<unsigned char>(rbuf[i]);

                // --- ESC 開頭：可能是 arrow / mouse / function / 或孤立 ESC
                if (c == 0x1B) {
                    // 先嘗試把後續 bytes 收集完整（非阻塞嘗試）
                    string seq;
                    seq.push_back((char)c);
                    // 把目前 buffer 中已有的後續 bytes 收起來
                    int j = i + 1;
                    while (j < n && (int)seq.size() < 64) { seq.push_back(rbuf[j]); j++; }

                    // 若目前 seq 看起來不完整（例如只有 "\x1b[" 還沒到最後），再嘗試用短 timeout 讀更多
                    if (seq.size() < 6) {
                        // 嘗試讀多點（不阻塞太久）
                        char more[64];
                        int mr = read_with_timeout(STDIN_FILENO, more, sizeof(more), 30); // 30ms
                        if (mr > 0) {
                            for (int k = 0; k < mr && (int)seq.size() < 128; ++k) seq.push_back(more[k]);
                        }
                    }

                    // 解析 seq：
                    // - CSI M ...  (X10)  : "\x1b[M" + 3 bytes
                    // - CSI < ... (SGR mouse) : "\x1b[<" ... 'M' or 'm'
                    // - CSI [ A/B/C/D (arrow) : "\x1b[A" etc or "\x1b[1;..." function keys
                    bool consumed = false;

                    // --- X10 mouse: ESC [ M Cb Cx Cy
                    if (seq.size() >= 6 && seq[1] == '[' && seq[2] == 'M') {
                        unsigned char cb = (unsigned char)seq[3];
                        unsigned char cx = (unsigned char)seq[4];
                        unsigned char cy = (unsigned char)seq[5];
                        int btn = cb - 32;
                        int col = (int)cx - 32 - 1;
                        int row = (int)cy - 32 - 1;
                        // 只有左鍵觸發 callback（btn & 0x03 == 0 表示左鍵按下）
                        if ((btn & 0x03) == 0) {
                            lock_guard<mutex> lock(cbMutex);
                            if (currentCallback) currentCallback(buttons.size() ? string() : string()); // placeholder
                            // We MUST call callback with the actual button name:
                            // iterate to find which button contains (col,row)
                            for (auto &b : buttons) {
                                if (pointInButton(col, row, b)) {
                                    if (currentCallback) currentCallback(b.name);
                                    buffer.clear();
                                    cursorPos = 0;
                                    mouseSync = false;
                                    break;
                                }
                            }
                        }
                        // consume 6 chars total
                        int consumedBytes = min(j - i, 6); // if we had all from rbuf or plus read_more
                        i += consumedBytes;
                        consumed = true;
                    }
                    // --- SGR mouse: ESC [ < Cb ; Cx ; Cy (M or m)
                    else if (seq.size() >= 4 && seq[1] == '[' && seq[2] == '<') {
                        // find terminal 'M' or 'm'
                        size_t posTerm = string::npos;
                        for (size_t k = 3; k < seq.size(); ++k) {
                            if (seq[k] == 'M' || seq[k] == 'm') { posTerm = k; break; }
                        }
                        if (posTerm == string::npos) {
                            // 可能不完整，再嘗試小等一下取更多
                            char more[64];
                            int mr = read_with_timeout(STDIN_FILENO, more, sizeof(more), 30);
                            if (mr > 0) {
                                for (int k = 0; k < mr; ++k) seq.push_back(more[k]);
                                for (size_t k = 3; k < seq.size(); ++k) {
                                    if (seq[k] == 'M' || seq[k] == 'm') { posTerm = k; break; }
                                }
                            }
                        }
                        if (posTerm != string::npos) {
                            // parse between [3..posTerm-1] as "Cb;Cx;Cy"
                            string inner = seq.substr(3, posTerm - 3);
                            // split by ';'
                            int cb = 0, cx = 0, cy = 0;
                            int parsed = sscanf(inner.c_str(), "%d;%d;%d", &cb, &cx, &cy);
                            if (parsed == 3) {
                                bool press = (seq[posTerm] == 'M');
                                // 左鍵 press -> callback
                                if (press && (cb & 0x03) == 0) {
                                    int col = cx - 1;
                                    int row = cy - 1;
                                    lock_guard<mutex> lock(cbMutex);
                                    for (auto &b : buttons) {
                                        if (pointInButton(col, row, b)) {
                                            if (currentCallback) currentCallback(b.name);
                                            buffer.clear();
                                            cursorPos = 0;
                                            mouseSync = false;
                                            break;
                                        }
                                    }
                                }
                            }
                            // consume seq length
                            int consumeCount = (int)min((size_t)n - i, posTerm + 1);
                            i += consumeCount;
                            consumed = true;
                        } else {
                            // 沒拿到完整序列 -> skip this ESC (保守處理) so it won't be considered an isolated ESC
                            // advance 1 and continue
                            i++;
                            consumed = true;
                        }
                    }
                    // --- Arrow / CSI like ESC [ A/B/C/D or function keys
                    else if (seq.size() >= 3 && seq[1] == '[' && ( (seq[2]>='A' && seq[2]<='D') || (seq[2]>='0' && seq[2]<='9') )) {
                        // 常見 arrow: ESC [ A/B/C/D
                        if (seq[2] >= 'A' && seq[2] <= 'D') {
                            // handle arrow for history/cursor only if kbEnabled
                            if (kbEnabled) {
                                if (seq[2] == 'A') { // up
                                    if (!history.empty() && historyIndex > 0) {
                                        historyIndex--;
                                        buffer = history[historyIndex];
                                        cursorPos = buffer.size();
                                        redrawAfterPrompt(cursorPos);
                                    }
                                } else if (seq[2] == 'B') { // down
                                    if (!history.empty() && historyIndex < (int)history.size() - 1) {
                                        historyIndex++;
                                        buffer = history[historyIndex];
                                    } else {
                                        buffer.clear();
                                        historyIndex = history.size();
                                    }
                                    cursorPos = buffer.size();
                                    redrawAfterPrompt(cursorPos);
                                } else if (seq[2] == 'C') { if (cursorPos < buffer.size()) { cursorPos++; redrawAfterPrompt(cursorPos); } }
                                else if (seq[2] == 'D') { if (cursorPos > 0) { cursorPos--; redrawAfterPrompt(cursorPos); } }
                            }
                            // consume 3 if available else 1
                            int consume = (j - i >= 3 ? 3 : 1);
                            i += consume;
                            consumed = true;
                        } else {
                            // function-like sequences: consume chunk until non-digit/';' and a final letter
                            // try to find final letter in seq
                            size_t posTerm = string::npos;
                            for (size_t k = 2; k < seq.size(); ++k) {
                                if ((seq[k] >= '@' && seq[k] <= '~')) { posTerm = k; break; }
                            }
                            if (posTerm != string::npos) {
                                int consumeCount = (int)min((size_t)n - i, posTerm + 1);
                                i += consumeCount;
                                consumed = true;
                            } else {
                                // 不完整，嘗試讀更多
                                char more[64];
                                int mr = read_with_timeout(STDIN_FILENO, more, sizeof(more), 25);
                                if (mr > 0) {
                                    for (int k = 0; k < mr; ++k) seq.push_back(more[k]);
                                    // 再尋找終結
                                    for (size_t k = 2; k < seq.size(); ++k) {
                                        if ((seq[k] >= '@' && seq[k] <= '~')) { posTerm = k; break; }
                                    }
                                    if (posTerm != string::npos) {
                                        int consumeCount = (int)min((size_t)n - i, posTerm + 1);
                                        i += consumeCount;
                                        consumed = true;
                                    } else {
                                        // give up -> consume 1
                                        i++;
                                        consumed = true;
                                    }
                                } else {
                                    i++;
                                    consumed = true;
                                }
                            }
                        }
                    }
                    // --- 純粹孤立 ESC (沒後續) -> 視為真 ESC
                    else {
                        // 如果 seq 只有 ESC（或後面沒有 '[' 或 '<'），我們視為孤立 ESC
                        // 向 stdin 嘗試非阻塞讀一小段判定是否真的孤立（等候很短）
                        char more[8];
                        int mr = read_with_timeout(STDIN_FILENO, more, sizeof(more), 12); // very short wait
                        if (mr <= 0) {
                            // 真正孤立 ESC
                            escDetected = true;
                            i++;
                            consumed = true;
                        } else {
                            // 剛才讀到的 bytes 可能屬於別的序列 — 把它放回到 local buffer 處理
                            // 先把更多數據擴充到 rbuf（若還有空間）
                            // 為簡單起見，把 these extra bytes 前移到 rbuf area by manipulating indices:
                            // We'll prepend them into the stream for next iteration:
                            // shift remaining bytes to right and insert more at current i+1 (complicated).
                            // 簡化處理：把 mr bytes直接當成一般字元處理（rare）
                            for (int k = 0; k < mr; ++k) {
                                // treat them as consumed by this branch -> just skip them
                            }
                            i += 1 + mr;
                            consumed = true;
                        }
                    }

                    if (consumed) continue;
                }

                // --- 普通字元 (非 ESC)
                if (!kbEnabled) { i++; continue; }

                if (c == '\r' || c == '\n') {
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
                    // after newline, spReset caller may update startCol if necessary
                    i++;
                    continue;
                }

                if (c == 127 || c == '\b') {
                    if (cursorPos > 0) {
                        buffer.erase(cursorPos - 1, 1);
                        cursorPos--;
                        redrawAfterPrompt(cursorPos);
                    }
                    i++;
                    continue;
                }

                // 功能鍵的二 byte 前綴 (mac read getch style) rarely happens here since we parse ESC sequences above
                // 一般可視為印字
                buffer.insert(cursorPos, 1, (char)c);
                cursorPos++;
                redrawAfterPrompt(cursorPos);
                i++;
            } // end while i<n
        } // end while running

    }); // end thread
}

void ManageInput::stopInput() {
    running = false;
    if (kbThread.joinable()) kbThread.join();
}

string ManageInput::getInput() {
    lock_guard<mutex> lock(inputMutex);
    return lastInput;
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