#pragma once
#include <string>
#include <functional>
class Config {
public:
    struct Settings {
        bool verbose = true;
    };
    Settings settings;
    struct Data {
        void save(const std::string& filePath, const std::string& data);
        void replace(const std::string& filePath, const std::string& targetData, const std::string& data);
        void load(const std::string& filePath, const std::string& targetData);
    };
    Data data;
};