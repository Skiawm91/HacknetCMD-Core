#include "config.h"
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
using namespace std;

string line;
vector<string> newLine;

void Config::Data::save(const string& filePath, const string& data) {
    line.clear();
    newLine.clear();
    if (!filesystem::exists("config")) {
        filesystem::create_directory("config");
    }
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        while(getline(inFile, line)) {
            newLine.push_back(line);
        }
        newLine.push_back(data);
        inFile.close();
    }
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        for (const auto &l : newLine) {
            outFile << l << endl;
        }
        outFile.close();
    }
}
void Config::Data::replace(const string& filePath, const string& targetData, const string& data) {
    line.clear();
    newLine.clear();
    if (!filesystem::exists("config")) {
        filesystem::create_directory("config");
    }
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        while(getline(inFile, line)) {
            if (line == targetData) {
                newLine.push_back(data);
            } else {
                newLine.push_back(line);
            }
        }
        inFile.close();
    }
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        for (const auto &l : newLine) {
            outFile << l << endl;
        }
        outFile.close();
    }
}

void Config::Data::load(const string& filePath, const string& targetData) {
    line.clear();
    if (!filesystem::exists("config")) {
        filesystem::create_directory("config");
    }
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        while(getline(inFile, line)) {
            if (line == targetData) {
                // idk
            }
        }
        inFile.close();
    }
}