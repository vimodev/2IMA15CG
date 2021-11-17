#include "intersection_cache.h"

bitset<MAX_EDGES> IntersectionCache::cache[MAX_EDGES];
Instance *IntersectionCache::instance = nullptr;
bool IntersectionCache::initialized = false;

// Set the given instance to be cached for intersections
void IntersectionCache::set_instance(Instance *instance) {
    IntersectionCache::instance = instance;
    IntersectionCache::populate_cache();
    IntersectionCache::initialized = true;
    cout << "Intersection cache filled." << endl;
}

bool IntersectionCache::get(int i, int j) {
    if (i > j) return get(j, i);
    return cache[i][j];
}

void IntersectionCache::populate_cache() {
    vector<Edge> *edges = instance->edges;
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < instance->m; i++) {
        for (int j = i + 1; j < instance->m; j++) {
            cache[i][j] = Edge::intersect(&edges->at(i), &edges->at(j));
        }
    }
}