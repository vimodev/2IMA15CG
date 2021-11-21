#include "../solvers.h"
#include "../intersection_cache.h"

Solution* GreedySolver::solve(Instance instance) {

    // Initialize colors
    vector<int> *colors = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        colors->push_back(-1);
    }

    int color = 0;
    bool done = false;
    int edge_index = 0;

    while (!done) {

        // Find first uncolored edge
        done = true;
        while (edge_index < instance.m) {
            if (colors->at(edge_index) != -1) {
                edge_index++;
                continue;
            }
            done = false;
            break;
        }
        if (done) break;

        // Set its color
        colors->at(edge_index) = color;

        // All non-intersecting edges get the same color
        for (int i = edge_index + 1; i < instance.m; i++) {
            // Make sure color is not set yet
            int c = colors->at(i);
            if (c != -1) continue;
            // If color not set, but intersect with current first edge its already no go
            if (IntersectionCache::intersects(edge_index, i)) continue;
            bool valid = true;
            // Go over all remaining edges that could have same color
            for (int j = edge_index + 1; j < i; j++) {
                // If they have same color, check for intersect
                bool colors_equal = colors->at(j) == color;
                if (colors_equal && IntersectionCache::intersects(i, j)) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                colors->at(i) = color;
            }
        }

        // Continue with the next color
        color++;

    }

    Solution *sol = new Solution(&instance);
    sol->num_colors = color;
    sol->colors = colors;
    return sol;
}
