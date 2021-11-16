#include <stdio.h>
#include <iostream>
#include <set>
#include <string.h>
#include "instance.h"
#include "solution.h"
#include <stdlib.h>
#include <filesystem>
#include <time.h>
#include <unistd.h>
#include <omp.h>

#include "intersection_cache.h"

using namespace std;

namespace fs = std::filesystem;

/**
 * Greedy solver that colors the first uncolored edge with a color
 * and then goes over all remaining uncolored edges to check for collisions with edges of the new color
 * and if there are none we give it the same color.
 */
Solution *greedy(Instance instance) {

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
            if (IntersectionCache::get(edge_index, i)) continue;
            bool valid = true;
            // Go over all remaining edges that could have same color
            for (int j = edge_index + 1; j < i; j++) {
                // If they have same color, check for intersect
                bool colors_equal = colors->at(j) == color;
                if (colors_equal && IntersectionCache::get(i, j)) {
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

Solution *naive (Instance instance, int debug) {
    // For each edge i to m,
    // find colors of intersecting edges
    // add these to set invalid_colors_for_i
    // then color edge i with the first color
    // that is not in invalid_colors_for_i
    std::vector<int> *color_allocation = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        color_allocation->push_back(-1);
    }

    std::set<int> invalid_colors_for_i = {};
    for (int i = 0; i < instance.m; i++) {
        std::set<int> invalid_colors_for_i = {};
        for (int j = 0; j < i; j++) {
            if (debug && Edge::intersect(&instance.edges->at(i), &instance.edges->at(j))) cout << "Intersection at edge " << i << " and " << j << endl;
            if (Edge::intersect(&instance.edges->at(i), &instance.edges->at(j)))
                invalid_colors_for_i.insert(color_allocation->at(j));
        }
        std::set<int>::iterator it = invalid_colors_for_i.begin();

        int c = 0;
        while (it != invalid_colors_for_i.end() && c == *it) {
            c++; it++;
        }
        color_allocation->at(i) = c;

        it = invalid_colors_for_i.begin();
        if (debug) cout << "Edge " << i << " cant choose colors: ";
        while (it != invalid_colors_for_i.end()) {
            if (debug) cout << *it << ",";
            it++;
        }
        if (debug) cout << endl << "\ttherefore chose color: " << c << endl;

        if (!debug) if (!((i+1) % (instance.m/4))) cout << "PROGRESS: " << (i*100)/instance.m << "%"<< endl;
    }

    Solution *sol = new Solution(&instance);

    sol->num_colors = *max_element(std::begin(*color_allocation), std::end(*color_allocation))+1;
    sol->colors = color_allocation;
    return sol;
} 

int main(int argc, char **argv) {
    // We want to be able to cancel out of OMP parallel sections
    if(!omp_get_cancellation()) {
        putenv((char*) "OMP_CANCELLATION=true");
        execv(argv[0], argv);
    }

    // Seed random
    srand (time(nullptr)); // NOLINT(cert-msc51-cpp)

    // for (const auto & entry : fs::directory_iterator("../instances")) {
    //     string p = entry.path();
    //     Instance inst(p);
    //     cout << "Solving " << inst.id << "..." << endl;
    //     Solution *sol = greedy(inst);
    //     if(sol->check_validity()) {
    //         cout << "Solution valid!" << endl;
    //     } else {
    //         cout << "Solution invalid!" << endl;
    //     }
    //     sol->to_file("../greedy_solutions/", true, "greedy");
    //     delete sol->colors;
    //     delete sol;
    //     delete inst.edges;
    //     delete inst.vertices;
    // }

    // Reading instance from json wokrs
    Instance inst("../instances/vispecn70501.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.edges->size() << " edges." << endl;
    IntersectionCache::set_instance(&inst);

    Solution *sol = greedy(inst);
    // Solution *sol = naive(inst, 0);

    if(sol->check_validity()) {
        cout << "Solution valid!" << endl;
    } else {
        cout << "Solution invalid!" << endl;
    }

    cout << "Colors used: " << sol->num_colors << endl;

    sol->to_file("", false, "");

    return 0;
}