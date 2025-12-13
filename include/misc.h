#pragma once
#include <string>
#include <vector>
class Misc {
public:
    std::string toLangName(const int code);
    struct Node_Modify {
        /* void rename(const std::vector<std::string>& path, const std::string&);  UNFINISH */
        void parameter(const std::string& name, const std::string& value);
    };
};