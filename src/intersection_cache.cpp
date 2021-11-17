#include "intersection_cache.h"

bitset<MAX_EDGES> IntersectionCache::cache[MAX_EDGES];
int IntersectionCache::counts[MAX_EDGES];
Instance *IntersectionCache::instance = nullptr;
bool IntersectionCache::initialized = false;

// Set the given instance to be cached for intersections
void IntersectionCache::set_instance(Instance *instance) {
    IntersectionCache::instance = instance;
    
    vector<Edge> *edges = instance->edges;
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < instance->m; i++) {
        for (int j = i + 1; j < instance->m; j++) {
            int intersect = Edge::intersect(&edges->at(i), &edges->at(j));
            if (intersect) {
                counts[i] += 1;
                counts[j] += 1;
            }
            cache[i][j] = intersect;
        }
    }

    IntersectionCache::initialized = true;
    cout << "Intersection cache filled." << endl;
}

bool IntersectionCache::intersects(int i, int j) {
    if (i > j) return cache[j][i];
    return cache[i][j];
}

int IntersectionCache::get_count(int i) {
    return counts[i];
}