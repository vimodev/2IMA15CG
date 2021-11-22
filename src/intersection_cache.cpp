#include "intersection_cache.h"

bitset<MAX_EDGES> IntersectionCache::cache[MAX_EDGES];
int IntersectionCache::counts[MAX_EDGES];
Instance *IntersectionCache::instance = nullptr;
bool IntersectionCache::initialized = false;
vector<int> *IntersectionCache::adjacency_list[MAX_EDGES];

// Set the given instance to be cached for intersections
void IntersectionCache::set_instance(Instance *pInstance) {
    IntersectionCache::instance = pInstance;

    // Initialize adjacency_lists with empty vectors.
    for (int i = 0; i < pInstance->m; i++) {
        adjacency_list[i] = new vector<int>;
    }

    vector<Edge> *edges = pInstance->edges;
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < pInstance->m; i++) {
        for (int j = i + 1; j < pInstance->m; j++) {
            int intersect = Edge::intersect(&edges->at(i), &edges->at(j));
            if (intersect) {
                counts[i] += 1;
                counts[j] += 1;
                adjacency_list[i]->push_back(j);
                adjacency_list[j]->push_back(i);
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

vector<int> *IntersectionCache::neighbours(int i) {
    return adjacency_list[i];
}