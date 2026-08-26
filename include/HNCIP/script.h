#pragma once
#include <string>
using namespace std;
class HNCIPScript {
    public:
        void CLEAR();
        void WAIT(const string& content);
        void PRINT(const string& content, bool save = false);
        void PRINTR(const string& content, bool save = false);
        void PRINTWFW(const string& content, bool save = false);
        void PRINTAT(const string& content, bool save = false);
        void PRINTAT_NB(const string& content, bool save = false);
        void PLAYAUDIO(const string& content);
        void GETINPUT(const string& content);
        void GETINPUTR(const string& content);
        void GETINPUTPWD(const string& content);
        void GETINPUTPWDR(const string& content);
        void GETINPUTNUL(const string& content);
        void GOTO(const string& targetFile, const string& content);
};