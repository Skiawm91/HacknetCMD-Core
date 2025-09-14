#define _HAS_STD_BYTE 0
#include "CHNScript.h"
#include "../../audio.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include <sstream>
using namespace std;

void CHNScript::PLAYAUDIO(const string& content) {
    string arg1;
    istringstream args(content);
    args >> arg1;
    StopAudio();
    PlayAudio(arg1);
}