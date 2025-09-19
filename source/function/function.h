#pragma once
#include <string>
using namespace std;
class Function {
public:
    struct Audio {
        void play(const string& audioFile);
        void stop();
    };
    Audio audio;
    struct CMD {
        void clear();
    };
    CMD cmd;
};