#define _HAS_STD_BYTE 0
#include "clearScreen.h"
#include "input/input.h"
#include "audio.h"
#include "logUI.h"
#include "hnasm/hnasm.h"
#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <unistd.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <termios.h>
#include <iostream>
#endif
#include <string>
#include <vector>
using namespace std;

#ifndef _WIN32
termios origTermios;
#endif
ManageInput mi;

int main(){
    #ifdef _WIN32
    // 初始化: 編碼
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // 初始化: 視窗大小/字體
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize;
    bufferSize.X = 120;
    bufferSize.Y = 30;
    SetConsoleScreenBufferSize(hOut, bufferSize);
    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = bufferSize.X - 1;
    windowSize.Bottom = bufferSize.Y - 1;
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    GetCurrentConsoleFontEx(hOut, FALSE, &cfi);
    wcscpy_s(cfi.FaceName, L"Cascadia Mono");
    SetCurrentConsoleFontEx(hOut, FALSE, &cfi);
    // 初始化: 標題
    SetConsoleTitleA("Hacknet for CMD");
    // 初始化: 終端機輸入
    mi.hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prevMode;
    GetConsoleMode(mi.hIn, &prevMode);
    DWORD mode = prevMode & ~ENABLE_QUICK_EDIT_MODE;
    mode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
    SetConsoleMode(mi.hIn, mode);
    #elif __APPLE__
    // 初始化: 資料夾位置
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        char path_copy[PATH_MAX];
        strncpy(path_copy, path, PATH_MAX);
        path_copy[PATH_MAX - 1] = '\0';
        char* dir = dirname(path_copy);
        chdir(dir);
    }
    // 初始化: 視窗大小
    cout << "\033[8;30;120t";
    cout.flush();
    // 初始化: 標題
    cout << "\033]0;Hacknet for CMD\007";
    // 初始化: 終端機輸入
    tcgetattr(STDIN_FILENO, &origTermios);
    termios raw = origTermios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    cout << "\033[?1000h";
    cout.flush();
    #endif
    #ifdef _WIN32
    mi.kbInput();
    mi.mouseInput();
    #elif __APPLE__
    mi.input();
    #endif
    int chse;
    extern string input;
    StopAudio();
    PlayAudio("AmbientDroneClipped.wav");
    while(true) {
        mi.kbDisable();
        chse = 0;
        HNASM("ui.chns", "LOGO");
        HNASM("ui.chns", "HOME");
        mi.btnAdd("PLAY", 2, 8, 20, 3);
        mi.btnAdd("QUIT", 2, 17, 20, 3);
        mi.cbCreate([&](const string& btnName){
            if (btnName == "PLAY") {
                chse = 1;
            }
            if (btnName == "QUIT") {
                chse = 4;
            }
        });
        mi.async(3);
        mi.btnDel(vector<string>{"PLAY", "QUIT"});
        mi.cbClean();
        switch(chse) {
            case 1:
                LogUI();
                break;
            case 4:
                {
                    while(true) {
                        chse = 0;
                        HNASM("ui.chns", "QUIT");
                        mi.btnAdd("QUIT", 1, 2, 20, 3);
                        mi.btnAdd("CANCEL", 1, 5, 20, 3);
                        mi.cbCreate([&](const string& btnName){
                            if (btnName == "QUIT") {
                                chse = 1;
                            }
                            if (btnName == "CANCEL") {
                                chse = 2;
                            }
                        });
                        mi.async(3);
                        mi.btnDel(vector<string>{"QUIT", "CANCEL"});
                        mi.cbClean();
                        if (chse == 1) {
                            exit(1);
                        } else if (chse == 2) {
                            break;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    return 0;
}