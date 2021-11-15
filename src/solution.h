#pragma once

#include <string.h>
#include <json.h>
#include <fstream>
#include <vector>
#include "instance.h"

using namespace std;
using json = nlohmann::json;

class Solution {
    public:
        Solution(Instance *instance);
        void to_file(string output_dir);
        bool check_validity();
        Instance *instance;
        int num_colors;
        vector<int> *colors;
};