#define _HAS_STD_BYTE 0
#include "data.h"
#include "function.h"
#include "console.h"
#include "UI.h"
#include "misc.h"
#include "input.h"
#include "HNCIP.h"
#include "os.h"
#include "discord_rpc.h"
#include "discord_register.h"
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
#include <filesystem>
using std::string;

// 初始化 class 部分
ManageInput mi;
Data dta;
Console con;
Function func;
Misc misc;
UserInterface UI;
hnfcOS os;
HNCInterPreter hncip;
DiscordRichPresence drp;

// 版本號
string ver = "0.3.0";
string verStage = "Beta 5";

int main() {
#ifdef _WIN32
    // 初始化: 編碼
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // 初始化: 基本視窗大小/字體
    con.resize(144, 36);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
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
    // 初始化: 基本視窗大小
    con.resize(120, 30);
    // 初始化: 標題
    cout << "\033]0;Hacknet for CMD\007";
    // 初始化: 終端機輸入
    termios origTermios;
    tcgetattr(STDIN_FILENO, &origTermios);
    termios raw = origTermios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    cout << "\033[?1000h";
    cout.flush();
    #endif
    // 初始化: 建立Data資料夾 / 載入配置檔案
    if (!filesystem::exists("data")) filesystem::create_directory("data");
    dta.cfg.reload();
    // 初始化: 輸入
    mi.initial();
    // 初始化: Discord Presence
    DiscordEventHandlers handlers = {};
    handlers.ready = [](const DiscordUser* user){ };
    Discord_Initialize("1428378052223697007", &handlers, 1, nullptr);
    string details;
    if (verStage == "Release") details = "Core Edition // " + ver;
    else details = "Core Edition // " + ver + " [" + verStage + "]";
    drp.details = details.c_str();
    drp.largeImageKey = "HNCMD_ICON";
    drp.largeImageText = "Hacknet for CMD";
    Discord_UpdatePresence(&drp);
    // 初始化結束
    UI.Home();
    return 0;
}