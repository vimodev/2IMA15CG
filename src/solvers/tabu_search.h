#pragma once

#include "../solution.h"

#define MAX_EDGES 100000
#define MAX_COLORS 1000

using namespace std;

class TabuSearch {
    public:
        Solution *solve(Solution* sol, int iterations);
    private:
        static int T[MAX_EDGES][MAX_COLORS];
        static int C[MAX_EDGES][MAX_COLORS];
};