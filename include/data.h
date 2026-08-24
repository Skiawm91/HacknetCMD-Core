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
        bool verboseBoot = true; // On
        bool dynamicLogo = true; // On
        int language = 0; // English
        int cmdsize = 0; // 1x
        int vt100color = 0; // Off
    private:
        Data* parent;
    };
    Config cfg;
};