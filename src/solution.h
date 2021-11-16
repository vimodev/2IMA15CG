#pragma once

#include <cstring>
#include <json.h>
#include <fstream>
#include <vector>
#include "instance.h"
#include <iostream>

using namespace std;
using json = nlohmann::json;

class Solution {
    public:
        explicit Solution(Instance *instance);
        void to_file(const string& output_dir, bool include_num=false, const string& alg="");
        bool check_validity();
        Instance *instance;
        int num_colors;
        vector<int> *colors;
};