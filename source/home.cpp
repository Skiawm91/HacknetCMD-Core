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
#endif
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include "porthack.h"
using namespace std;

#ifndef _WIN32
termios origTermios;
#endif
ManageInput mi;

int main(){
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    SetConsoleTitleA("Hacknet For CMD");
    mi.hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD prevMode;
    GetConsoleMode(mi.hIn, &prevMode);
    DWORD mode = prevMode & ~ENABLE_QUICK_EDIT_MODE;
    mode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
    SetConsoleMode(mi.hIn, mode);
    #elif __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        char path_copy[PATH_MAX];
        strncpy(path_copy, path, PATH_MAX);
        path_copy[PATH_MAX - 1] = '\0';
        char* dir = dirname(path_copy);
        chdir(dir);
    }
    cout << "\033]0;Hacknet For CMD\007";
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
                mouseSync = false;
            }
            if (btnName == "QUIT") {
                chse = 4;
                mouseSync = false;
            }
        });
        mouseSync = true;
        mi.async(3);
        mi.btnDel(vector<string>{"PLAY", "QUIT"});
        mi.cbClean();
        switch(chse) {
            case 1:
                LogUI();
                break;
            case 4:
                {
                    string yn;
                    #ifdef _WIN32
                    cls();
                    #elif __APPLE__
                    system("clear");
                    #endif
                    while(true) {
                        mi.kbEnable();
                        cout << "Are you sure to quit Hacknet? (y/n)\n";
                        kbPrompt = "choose: ";
                        mi.spReset();
                        mi.async(2);
                        if (escDetected) {
                            escDetected = false;
                        } else if (enterDetected) {
                            enterDetected = false;
                            yn = mi.getInput();
                        }
                        if (yn == "y") {
                            exit(1);
                        } else if (yn == "n") {
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