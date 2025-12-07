#pragma once
#include <string>
#include <vector>
using std::vector, std::string;

class Function {
public:
    struct Audio {
        void play(const string& threadName, const vector<string>& fileNames, const string& filePath = "assets/musics/");
        void playL(const string& threadName, const vector<string>& fileNames, const string& filePath = "assets/musics/");
        void stop(const string& threadName);
        void stop();
    };
    Audio audio;
};