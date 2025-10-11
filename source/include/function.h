#pragma once
#include <string>
#include <vector>
using namespace std;

class Function {
public:
    struct Audio {
        void play(const string& threadName, const vector<string>& fileNames);
        void playL(const string& threadName, const vector<string>& fileNames);
        void stop(const string& threadName);
        void stop();
    };
    Audio audio;
};