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
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        while(getline(inFile, line)) {
            newLine.push_back(line);
        }
        newLine.push_back(data);
        inFile.close();
    } else {
        newLine.push_back(data);
    }
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        for (const auto &l : newLine) {
            outFile << l << endl;
        }
        outFile.close();
    }
}

void Config::Data::del(const string& filePath, const string& targetData) {
    line.clear();
    newLine.clear();
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        bool found = false;
        while(getline(inFile, line)) {
            if (line != targetData) {
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

void Config::Data::replace(const string& filePath, const string& targetData, const string& data) {
    line.clear();
    newLine.clear();
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        bool found = false;
        while(getline(inFile, line)) {
            if (line == targetData) {
                found = true;
                newLine.push_back(data);
            } else {
                newLine.push_back(line);
            }
        }
        if (!found) newLine.push_back(data);
        inFile.close();
    } else {
        newLine.push_back(data);
    }
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        for (const auto &l : newLine) {
            outFile << l << endl;
        }
        outFile.close();
    }
}

void Config::Data::replace(const string& filePath, const int targetLine, const string& data) {
    line.clear();
    newLine.clear();
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        bool found = false;
        int i = 0;
        while(getline(inFile, line)) {
            if (i == targetLine) {
                found = true;
                newLine.push_back(data);
            } else { 
                newLine.push_back(line);
            }
            ++i;
        }
        if (!found) newLine.push_back(data);
        inFile.close();
    } else {
        newLine.push_back(data);
    }
    ofstream outFile(filePath);
    if (outFile.is_open()) {
        for (const auto &l : newLine) {
            outFile << l << endl;
        }
        outFile.close();
    }
}

void Config::Data::load(const string& filePath, const vector<string>& targetData) {
    line.clear();
    this->loaded = false;
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        while(getline(inFile, line)) {
            int i = 0;
            for (const auto &td : targetData) {
                if (line == td) {
                    this->loaded = true;
                    this->loadNumber = i;
                    break;
                }
                ++i;
            }
        }
        inFile.close();
    }
}

string Config::Data::load(const string& filePath, const int targetLine) {
    line.clear();
    this->loaded = false;
    ifstream inFile(filePath);
    if (inFile.is_open()) {
        int i = 0;
        while(getline(inFile, line)) {
            if (i == targetLine) {
                this->loaded = true;
                break;
            }
            ++i;
        }
        inFile.close();
    }
    return line;
}