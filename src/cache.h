#pragma once

#include <bits/stdc++.h>
#include <iostream>

#include "instance.h"
#include <vector>

using namespace std;

class Cache {
    public:
        static void set_instance(Instance *pInstance);
        static std::tuple<std::chrono::duration<float, std::milli>, std::chrono::duration<float, std::milli>> benchmark(Instance *pInstance);
        static bool intersects(int i, int j);
        static int get_count(int i);
        static bool initialized;
        static vector<int> *neighbours(int i);
        static Instance *instance;
        static vector<int> *adjacency_list[MAX_EDGES];
        static bitset<MAX_EDGES> cache[MAX_EDGES];
        static int counts[MAX_EDGES];
        static void setIntersect(int i, int j);
        
};