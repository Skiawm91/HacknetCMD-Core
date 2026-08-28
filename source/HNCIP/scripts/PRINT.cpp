#define _HAS_STD_BYTE 0
#include "HNCIP/script.h"
#include "input.h"
#include "console.h"
#include "data.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
using namespace std;

extern Console con;
extern Data dta;
#ifndef _WIN32
inline void Sleep(const int& ms) {usleep(ms * 1000);}
#endif

void HNCIPScript::PRINT(const string& content, bool save) {
    if (content.empty() && save) con.println().save();
    else if (save) con.println(content).save();
    else if (content.empty()) con.println();
    else con.println(content);
}

void HNCIPScript::PRINTR(const string& content, bool save) {
    if (content.empty() && save) {
        con.print("\r").save();
    } else if (save) {
        con.print("\r" + content).save();
    } else if (content.empty()) {
        con.print("\r");
    } else {
        con.print("\r" + content);
    }
}

void HNCIPScript::PRINTWFW(const string& content, bool save) {
    vector<string> text;
    istringstream iss(content);
    string word;
    while (iss >> word) {
        if (word == "␣") {
            text.push_back(" ");
        } else {
            text.push_back(word);
        }
    }

    // 組合出特效播完後的「完整字串」（把單字跟空白組回來）
    string fullSentence = "";
    srand((unsigned int)time(nullptr));
    
    for (size_t i = 0; i < text.size(); ++i) {
        con.print(text[i]);
        fullSentence += text[i]; // 這裡面已經自然包含被還原的 " " 空白了！
        Sleep(rand() % 21 + 30);
    }
    con.println();
    // 增加一筆流式紀錄
    if (save) {
        con.addRecord({ 
            -1, -1, 
            fullSentence + "\n", 
            con.getFg(), con.getBg(), 
            false, true 
        });
    }
}

void HNCIPScript::PRINTAT(const string& content, bool save, bool noEraseEOL) {
    int x = 0, y = 0;
    string xS, yS, str;
    istringstream iss(content);
    iss >> xS >> yS;
    getline(iss, str);
    str.erase(0,1);
    try {
        x = stoi(xS);
        y = stoi(yS);
    } catch (...) { return; }
    if (!str.empty() && save) con.printAt(x, y, str).save();
    else if (!str.empty()) con.printAt(x, y, str);
    else if (save) con.printAt(x, y, " ").save();
    else con.printAt(x, y, " ");
    // printAt 已經自動重置，不需要再呼叫
}

void HNCIPScript::PRINTAT_NB(const string& content, bool save, bool noEraseEOL) {
    int x = 0, y = 0;
    string xS, yS, str;
    istringstream iss(content);
    iss >> xS >> yS;
    getline(iss, str);
    str.erase(0,1);
    try {
        x = stoi(xS);
        y = stoi(yS);
    } catch (...) { return; }
    if (str.empty()) str = " ";
    if (save && noEraseEOL) con.printAt(x, y, str).noBack().noEraseEOL().save();
    else if (noEraseEOL) con.printAt(x, y, str).noBack().noEraseEOL();
    else if (save) con.printAt(x, y, str).noBack().save();
    else con.printAt(x, y, str).noBack();
    // noBack 已經自動重置，不需要再呼叫
}