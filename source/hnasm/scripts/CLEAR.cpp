#define _HAS_STD_BYTE 0
#include "ASMScript.h"
#include "../../clearScreen.h"
#ifdef _WIN32
#include <windows.h>
#endif

void HNScript::CLEAR(){
    #ifdef _WIN32
    cls();
    #else
    system("clear");
    #endif
}