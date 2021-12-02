#include "../solvers.h"
#include "../cache.h"

// DSatur solver for graph coloring, based on vertex color 'saturation'
// Samuel Oosterholt
Solution *DSaturSolver::solve(Instance instance) {
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> distr(0, this->N-1); // define the range

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
        verticesSaturation->at(i) = numeric_limits<int>::min();

        // If the vertex has no neighbours set it to 0
        if (Cache::get_count(i) == 0) {
            parts->at(0).insert(i);
            sol->colors->at(i) = 0;
            goto skip;
        }

        // Else we add it to the first possible part.
        for (int p = 0; p < parts->size(); p++) {
            // If i has a neighbour in part p we skip.
            for (int v : parts->at(p)) if (Cache::intersects(i, v)) goto next;

            // First suitable neighbour found.
            parts->at(p).insert(i);
            sol->colors->at(i) = p;

            // Increase saturation of neighbours
            for (int n : *Cache::neighbours(i)) {
                verticesSaturation->at(n) = verticesSaturation->at(n) + 1;
            }

            goto skip;

            next:;
        }

        if (parts->size() < this->N) {
            parts->push_back({i});
            sol->colors->at(i) = parts->size()-1;
        } else {
            int c = distr(gen);
            parts->at(c).insert(i);
            sol->colors->at(i) = c;
        }

        // Increase saturation of neighbours
        for (int n : *Cache::neighbours(i)) {
            verticesSaturation->at(n) = verticesSaturation->at(n) + 1;
        }

        skip:;
    }

    sol->num_colors = parts->size();

    return sol;
}