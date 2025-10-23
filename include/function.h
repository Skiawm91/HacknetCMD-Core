#pragma once
#include <string>
#include <vector>
using namespace std;

class Function {
public:
    struct Audio {
        void play(const string& threadName, const vector<string>& fileNames, const int type = 0);
        void playL(const string& threadName, const vector<string>& fileNames, const int type = 0);
        void stop(const string& threadName);
        void stop();
    };
    Audio audio;
};