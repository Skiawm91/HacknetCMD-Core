#pragma once
#include <string>
#include <optional>
#include <vector>
using namespace std;
void HNASM(const string& fileName, const string& partName, const optional<vector<string>>& targetVar = nullopt, const optional<vector<string>>& returnText = nullopt);