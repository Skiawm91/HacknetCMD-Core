#define _HAS_STD_BYTE 0
#include "HNCIP/script.h"
#include "console.h"

extern Console con;

void HNCIPScript::COLOR(const string& content, bool noFill, bool once) {
    if (content.empty()) return;
    if (noFill && once) con.setColor(content).noFill().once();
    else if (noFill) con.setColor(content).noFill();
    else if (once) con.setColor(content).once();
    else con.setColor(content);
}

void HNCIPScript::COLORBG(const string& content, bool noFill, bool once) {
    if (content.empty()) return;
    if (noFill && once) con.setColorBg(content).noFill().once();
    else if (noFill) con.setColorBg(content).noFill();
    else if (once) con.setColorBg(content).once();
    else con.setColorBg(content);
}