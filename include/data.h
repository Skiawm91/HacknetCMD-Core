#pragma once
#include <string>
#include <vector>
class Data {
public:
    Data() : cfg(this) {}
    bool loaded;
    int loadNumber;
    void save(const std::string& filePath, const std::string& data);
    void del(const std::string& filePath, const std::string& targetData);
    void replace(const std::string& filePath, const std::string& targetData, const std::string& data);
    void replace(const std::string& filePath, const int targetLine, const std::string& data);
    void load(const std::string& filePath, const std::vector<std::string>& targetData);
    std::string load(const std::string& filePath, const int targetLine);
    struct Config {
    public:
        Config(Data* p) : parent(p) {}
        void reload();
        // VT100 Configuration (Windows)
        bool vt100Color = 1; // On
        bool vt100Resize = 1; // On
        bool vt100Clear = 1; // On
        // VT100 Configuration (POSIX)
        bool true24BitColor = 1; // On
        // General
        bool verboseBoot = true; // On
        bool dynamicLogo = true; // On
        int language = 0; // English
        int cmdsize = 0; // 144*36
    private:
        Data* parent;
    };
    Config cfg;
};