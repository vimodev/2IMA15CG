#pragma once

#include <bits/stdc++.h>
#include <iostream>

#include "instance.h"
#include <vector>

#define MAX_EDGES 100000

using namespace std;

class Cache {
    public:
        static void set_instance(Instance *pInstance);
        static bool intersects(int i, int j);
        static int get_count(int i);
        static bool initialized;
        static vector<int> *neighbours(int i);
        static Instance *instance;

    private:
        static vector<int> *adjacency_list[MAX_EDGES];
        static bitset<MAX_EDGES> cache[MAX_EDGES];
        static int counts[MAX_EDGES];
};