#include "../searchers.h"
#include "../cache.h"

// Iterative Searcher for graph coloring
// Maxim Snoep
Solution *IterativeSearcher::search(Solution* sol, int iterations) {
    int n = sol->instance->m;
    int k = sol->num_colors;

    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> distr_n(0, n-1); // define the range
    uniform_int_distribution<> distr_k(0, k-1); // define the range

    int index;

    int new_color;
    int old_color;
    
    int prev;
    int new_clashes;
    int old_clashes = sol->get_clashes();

    for (int i = 0; i < iterations; i++) {

        index = distr_n(gen);        
        new_color = distr_k(gen);

        old_color = sol->colors->at(index);

        sol->colors->at(index) = new_color;
        new_clashes = sol->get_clashes();

        if (old_clashes < new_clashes) {
            sol->colors->at(index) = old_color;
        } else {
            old_clashes = new_clashes;
        }


        if (prev != old_clashes) {
            cout << "[INFO] Iteration " << (i+1) << "/" << iterations << " of II." << endl;
            cout << "[INFO] Current number of clashes: " << old_clashes << "." << endl;
        }
        prev = old_clashes;

        if (new_clashes == 0) break;
    }



    return sol;
}