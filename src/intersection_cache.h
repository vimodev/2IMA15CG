#pragma once

#include <bits/stdc++.h>
#include <iostream>

#include "instance.h"
#include <vector>

#define MAX_EDGES 100000

using namespace std;

class IntersectionCache {
    public:
        static void set_instance(Instance *instance);
        static bool intersects(int i, int j);
        static int get_count(int i);
        static bool initialized;
        static Instance *instance;

    private:
        static bitset<MAX_EDGES> cache[MAX_EDGES];
        static int counts[MAX_EDGES];
        static void populate_cache();
};