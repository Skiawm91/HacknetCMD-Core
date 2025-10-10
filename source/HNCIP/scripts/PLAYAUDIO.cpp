#define _HAS_STD_BYTE 0
#include "HNCIP/script.h"
#include "function.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include <sstream>
using namespace std;

extern Function func;

void HNCIPScript::PLAYAUDIO(const string& content) {
    string arg1;
    istringstream args(content);
    args >> arg1;
    func.audio.stop();
    func.audio.play(arg1);
}