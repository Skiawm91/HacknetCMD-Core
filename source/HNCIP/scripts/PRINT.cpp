#define _HAS_STD_BYTE 0
#include "HNCIP/script.h"
#include "input.h"
#include "console.h"
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
#ifndef _WIN32
inline void Sleep(const int& ms) {usleep(ms * 1000);}
#endif

void HNCIPScript::PRINT(const string& content) {
    if (content.empty()) {
        cout << endl;
    } else {
        cout << content << endl;
    }
}

void HNCIPScript::PRINTR(const string& content) {
    if (content.empty()) {
        cout << "\r";
        cout.flush();
    } else {
        cout << "\r" << content;
        cout.flush();
    }
}

void HNCIPScript::PRINTWFW(const string& content) {
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
    srand((unsigned int)time(nullptr));
    for (const string& t : text) {
        cout << t << flush;
        Sleep(rand() % 21 + 30);
    }
    cout << endl;
}

void HNCIPScript::PRINTAT(const string& content) {
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
    if (!str.empty()) con.printAt(x, y, str);
    else con.printAt(x, y, " ");
}

void HNCIPScript::PRINTAT_NB(const string& content) {
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
    if (!str.empty()) con.pae.noBack(x, y, str);
    else con.pae.noBack(x, y, " ");
}