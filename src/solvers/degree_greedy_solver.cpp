#include "../solvers.h"
#include "../cache.h"

Solution *DegreeGreedySolver::solve(Instance instance) {
    Solution *sol = new Solution(&instance);
    sol->colors = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        sol->colors->push_back(-1);
    }

    vector< set<int> >* parts = new vector< set<int> >;
    parts->push_back({});

    vector<int>* vertices = new vector<int>;
    for (int i = 0; i < instance.m; i++) {
        vertices->push_back(Cache::get_count(i));
    }

    for (int _ : *vertices) {
        int i = max_element(vertices->begin(), vertices->end()) - vertices->begin();
        vertices->at(i) = numeric_limits<int>::min();

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

    sol->num_colors = parts->size();

    return sol;
}