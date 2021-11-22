#include "tabu_search.h"
#include "../intersection_cache.h"

int TabuSearch::T[MAX_EDGES][MAX_COLORS];
int TabuSearch::C[MAX_EDGES][MAX_COLORS];

Solution *TabuSearch::solve(Solution* sol, int iterations) {
    int n = sol->instance->m;
    int k = sol->num_colors;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; i++) {
        if (IntersectionCache::get_count(i) == 0) {
            for (int j = 0; j < n; j++) {
                C[i][j] = numeric_limits<int>::max();
            }
        }
        for (int j : *IntersectionCache::neighbours(i)) {
            C[i][sol->colors->at(j)] = C[i][sol->colors->at(j)] + 1;
        }
    }

    vector< set<int> >* parts = new vector< set<int> >;
    for (int _ = 0; _ < k; _++) parts->push_back({});

    for (int i = 0; i < n; i++) {
        int c = sol->colors->at(i);
        parts->at(c).insert(i);
    }

    int cur_c = sol->get_clashes();
    vector<tuple<int,int>> *bests;
    int best;
    int best_c;
    int best_i;
    int best_j;
    for(int iteration = 0; iteration < iterations; iteration++) {
        bests = new vector<tuple<int,int>>;
        best_c = numeric_limits<int>::max();

        for (int i = 0; i < n; i++) {
            if (IntersectionCache::get_count(i) == 0) continue;
            for (int j = 0; j < k; j++) {
                if (j == sol->colors->at(i)) continue;
                int clashes = cur_c + C[i][j] - C[i][sol->colors->at(i)];
                if (T[i][j] > iteration && clashes != 0) continue;
                if (clashes < best_c) {
                    bests->clear();
                }
                if (clashes <= best_c) {
                    bests->push_back(make_tuple(i,j));
                    best_c = clashes;
                }
            }
        }

        random_device rd; // obtain a random number from hardware
        mt19937 gen(rd()); // seed the generator
        uniform_int_distribution<> distr(0, bests->size()-1); // define the range
        best = distr(gen);
        best_i = get<0>(bests->at(best));
        best_j = get<1>(bests->at(best));

        for (int u : *IntersectionCache::neighbours(best_i)) {
            C[u][sol->colors->at(best_i)] = C[u][sol->colors->at(best_i)] - 1;
            C[u][best_j] = C[u][best_j] + 1;
        }

        parts->at(sol->colors->at(best_i)).erase(best_i);
        parts->at(best_j).insert(best_i);
        sol->colors->at(best_i) = best_j;

        cur_c = sol->get_clashes();
        T[best_i][best_j] = iteration + cur_c*cur_c;
        
        if (iteration % 100 == 0) {
            cout << "Iteration: " << iteration << " for num_colors: " << k << endl;
            cout << "NR OF CLASHES: " << cur_c << endl << endl;
        }
        if (cur_c == 0) break;
    }

    sol->num_colors = parts->size();

    return sol;
}