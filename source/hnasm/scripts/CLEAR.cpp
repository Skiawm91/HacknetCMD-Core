#define _HAS_STD_BYTE 0
#include "CHNScript.h"
#include "../../clearScreen.h"
#ifdef _WIN32
#include <windows.h>
#endif

void CHNScript::CLEAR(){
    cls();
}