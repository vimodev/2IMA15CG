#include <stdio.h>
#include <iostream>
#include <string.h>
#include "instance.h"
#include "solution.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

using namespace std;

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
        Edge *edge = &instance.edges->at(edge_index);
        
        // Set its color
        colors->at(edge_index) = color;

        // All non-intersecting edges get the same color
        for (int i = edge_index + 1; i < instance.m; i++) {
            // Make sure color is not set yet
            int c = colors->at(i);
            if (c != -1) continue;
            // If color not set, but intersect with current first edge its already no go
            if (Edge::intersect(edge, &instance.edges->at(i))) continue;
            bool valid = true;
            // Go over all remaining edges that could have same color
            for (int j = edge_index + 2; j < i; j++) {
                // If they have same color, check for intersect
                bool colors_equal = colors->at(j) == color;
                if (colors_equal && Edge::intersect(&instance.edges->at(i), &instance.edges->at(j))) {
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

int main(int argc, char **argv) {
    // We want to be able to cancel out of OMP parallel sections
    if(!omp_get_cancellation()) {
        putenv("OMP_CANCELLATION=true");
        execv(argv[0], argv);
    }

    // Seed random
    srand (time(NULL));

    // Reading instance from json wokrs
    Instance inst("reecn3382.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.edges->size() << " edges." << endl;

    Solution *sol = greedy(inst);

    if(sol->check_validity()) {
        cout << "Solution valid!" << endl;
    } else {
        cout << "Solution invalid!" << endl;
    }

    cout << "Colors used: " << sol->num_colors << endl;

    sol->to_file("");

    return 0;
}