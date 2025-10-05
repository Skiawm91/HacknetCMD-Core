#pragma once
#include <string>
#include <optional>
#include <vector>
#include <tuple>
using namespace std;
class HNASM {
public:
    void script(const string& fileName, const string& partName, const optional<vector<string>>& targetVar = nullopt, const optional<vector<string>>& returnText = nullopt);
    tuple<string, string, string, int, vector<string>, vector<int>, bool> node(const string& fileName);
};