#include <stdio.h>
#include <iostream>
#include <set>
#include <string.h>
#include <stdlib.h>
#include <filesystem>
#include <time.h>
#include <unistd.h>
#include <omp.h>
#include <random>

#include "intersection_cache.h"
#include "instance.h"
#include "solution.h"
#include "adjacency_list.h"

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

Solution *improved_greedy (Instance instance, int N) {
    Solution *sol = new Solution(&instance);
    sol->colors = new vector<int>;
    vector<int>* vertices = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        sol->colors->push_back(-1);
        vertices->push_back(i);
    }

    for (int iteration = 0; iteration < N; iteration++) {
        vector< set<int> >* parts = new vector< set<int> >;
        parts->push_back({});
    
        for (int i = 0; i < instance.m; i++) {
            if (AdjacencyList::neighbours(i) == 0) {
                parts->at(0).insert(i);
                sol->colors->at(i) = 0;
                goto skip;
            }

            for (int p = 0; p < parts->size(); p++) {
                for (int v : parts->at(p)) if (IntersectionCache::get(i, v)) goto next;
                parts->at(p).insert(i);
                sol->colors->at(i) = p;
                goto skip;
                next:;
            }
            parts->push_back({i});
            sol->colors->at(i) = parts->size()-1;
            skip:;
        }

        vertices = new vector<int>;
        random_shuffle(parts->begin(), parts->end());
        for (int p = 0; p < parts->size(); p++) for (int v : parts->at(p)) vertices->push_back(v);
        sol->num_colors = parts->size();
    }

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
    AdjacencyList::initialize();

    Solution *sol = improved_greedy(inst, 10);

    cout << "Solution found. Colors used: " << sol->num_colors << endl;
    cout << "Checking validity..." << endl;

    if(sol->check_validity()) {
        cout << "Solution valid!" << endl;
    } else {
        cout << "Solution invalid!" << endl;
    }

    sol->to_file("", false, "");

    return 0;
}