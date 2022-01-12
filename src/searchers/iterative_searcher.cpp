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

    for (int i = 0; i < iterations; i++) {

        int index = distr_n(gen);        
        int new_color = distr_k(gen);

        int old_clashes = sol->get_clashes();
        int old_color = sol->colors->at(index);

        sol->colors->at(index) = new_color;
        int new_clashes = sol->get_clashes();

        if (old_clashes < new_clashes) {
            sol->colors->at(index) = old_color;
        } else {
            old_clashes = new_clashes;
        }

        if (i % 100 == 0) {
            cout << "Iteration: " << i << " for num_colors: " << k << endl;
            cout << "NR OF CLASHES: " << new_clashes << endl << endl;
        }
        if (new_clashes == 0) break;
    }



    return sol;
}