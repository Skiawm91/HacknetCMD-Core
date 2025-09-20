#define _HAS_STD_BYTE 0
#include "CHNScript.h"
#include "../../function/function.h"
#ifdef _WIN32
#include <windows.h>
#endif

extern Function func;

void CHNScript::CLEAR(){
    func.cmd.clear();
}