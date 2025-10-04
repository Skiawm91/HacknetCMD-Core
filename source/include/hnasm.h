#pragma once
#include <string>
#include <optional>
using namespace std;
void HNASM(const string& fileName, const string& partName, const optional<string>& targetVar = nullopt, const optional<string>& returnText = nullopt);