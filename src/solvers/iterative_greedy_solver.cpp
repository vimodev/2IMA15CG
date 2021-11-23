#include "../solvers.h"
#include "../cache.h"

Solution *IterativeGreedySolver::solve(Instance instance) {
    Solution *sol = new Solution(&instance);
    sol->colors = new vector<int>;
    vector<int>* vertices = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        sol->colors->push_back(-1);
        vertices->push_back(i);
    }

    for (int iteration = 0; iteration < this->N; iteration++) {
        vector< set<int> >* parts = new vector< set<int> >;
        parts->push_back({});

        for (int i = 0; i < instance.m; i++) {
            if (Cache::get_count(i) == 0) {
                parts->at(0).insert(i);
                sol->colors->at(i) = 0;
                goto skip;
            }

            for (int p = 0; p < parts->size(); p++) {
                for (int v : parts->at(p)) if (Cache::intersects(i, v)) goto next;
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