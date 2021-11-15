#pragma once

#include <string.h>
#include <json.h>
#include <fstream>
#include <vector>

using namespace std;
using json = nlohmann::json;

struct Solution {
    void toFile(string output_dir);
    string type;
    string instance;
    int num_colors;
    vector<int> *colors;
};