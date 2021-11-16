#pragma once

#include <string.h>
#include <json.h>
#include <fstream>
#include <vector>
#include "instance.h"
#include <iostream>

using namespace std;
using json = nlohmann::json;

class Solution {
    public:
        Solution(Instance *instance);
        void to_file(string output_dir, bool include_num=false, string alg="");
        bool check_validity();
        Instance *instance;
        int num_colors;
        vector<int> *colors;
};