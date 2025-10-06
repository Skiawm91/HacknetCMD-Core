#pragma once
#undef goto
#include <string>
using namespace std;
class CHNScript {
    public:
        void CLEAR();
        void WAIT(const string& content);
        void PRINT(const string& content);
        void PRINTR(const string& content);
        void PRINTWFW(const string& content);
        void PRINTAT(const string& content);
        void PLAYAUDIO(const string& content);
        void GETINPUT(const string& content);
        void GETINPUTR(const string& content);
        void GETINPUTPWD(const string& content);
        void GETINPUTPWDR(const string& content);
        void GETINPUTNUL(const string& content);
        void GOTO(const string& targetFile, const string& content);
};