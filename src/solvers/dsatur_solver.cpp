#include "../solvers.h"
#include "../intersection_cache.h"

Solution *DSaturSolver::solve(Instance instance) {
    auto *sol = new Solution(&instance);

    // Set every color to -1
    sol->colors = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        sol->colors->push_back(-1);
    }

    // Parts are the coloring segments.
    auto* parts = new vector< set<int> >;
    parts->push_back({});

    // Keep traack of saturation.
    auto* verticesSaturation = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        verticesSaturation->push_back(0);
    }

    // Loop over all vertices.
    for (int _ : *verticesSaturation) {
        // Get index of element with max saturation.
        int i = max_element(verticesSaturation->begin(), verticesSaturation->end()) - verticesSaturation->begin();
        verticesSaturation->at(i) = -999999;

        // If the vertex has no neighbours set it to 0
        if (IntersectionCache::get_count(i) == 0) {
            parts->at(0).insert(i);
            sol->colors->at(i) = 0;
            goto skip;
        }

        // Else we add it to the first possible part.
        for (int p = 0; p < parts->size(); p++) {
            // If i has a neighbour in part p we skip.
            for (int v : parts->at(p)) if (IntersectionCache::intersects(i, v)) goto next;

            // First suitable neighbour found.
            parts->at(p).insert(i);
            sol->colors->at(i) = p;

            // Increase saturation of neighbours
            for (int n : *IntersectionCache::neighbours(i)) {
                verticesSaturation->at(n) = verticesSaturation->at(n) + 1;
            }

            goto skip;

            next:;
        }

        parts->push_back({i});
        sol->colors->at(i) = parts->size()-1;

        // Increase saturation of neighbours
        for (int n : *IntersectionCache::neighbours(i)) {
            verticesSaturation->at(n) = verticesSaturation->at(n) + 1;
        }

        skip:;
    }

    sol->num_colors = parts->size();

    return sol;
}