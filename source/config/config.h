#pragma once
#include <string>
#include <vector>
class Config {
public:
    struct Settings {
        bool verbose = true; // On
        int language = 0; // English
        int cmdsize = 0; // 1x
    };
    Settings settings;
    struct Data {
        bool loaded;
        int loadNumber;
        void save(const std::string& filePath, const std::string& data);
        void del(const std::string& filePath, const std::string& targetData);
        void replace(const std::string& filePath, const std::string& targetData, const std::string& data);
        void load(const std::string& filePath, const std::vector<std::string>& targetData);
        std::string load(const std::string& filePath, const int targetLine);
    };
    Data data;
    void reload();
};