#pragma once

#include <iostream>
#include <vector>

#include "intersection_cache.h"

using namespace std;

class AdjacencyList {
    public:
        static void initialize();
        static bool edge_exists(int i, int j);
        static vector<int> *neighbours(int i);
        static bool initialized;
    private:
        static vector<int> *adjacency_list[MAX_EDGES];
        static void populate();
};