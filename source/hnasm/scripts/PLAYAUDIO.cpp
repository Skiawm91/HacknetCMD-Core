#define _HAS_STD_BYTE 0
#include "CHNScript.h"
#include "../../function/function.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include <sstream>
using namespace std;

extern Function func;

void CHNScript::PLAYAUDIO(const string& content) {
    string arg1;
    istringstream args(content);
    args >> arg1;
    func.audio.stop();
    func.audio.play(arg1);
}