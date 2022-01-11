#include "cache.h"
#include "sweep.h"

// Implementation to store intersection results for rapid access
// Vincent Moonen

bitset<MAX_EDGES> Cache::cache[MAX_EDGES];
int Cache::counts[MAX_EDGES];
Instance *Cache::instance = nullptr;
bool Cache::initialized = false;
vector<int> *Cache::adjacency_list[MAX_EDGES];

// Set the given instance to be cached for intersections
// Vincent Moonen
void Cache::set_instance(Instance *pInstance) {
    Cache::instance = pInstance;

    // Initialize adjacency_lists with empty vectors.
    for (int i = 0; i < pInstance->m; i++) {
        adjacency_list[i] = new vector<int>;
    }

    vector<Edge> *edges = pInstance->edges;

    Sweepline::sweep(edges);
    int sum = 0;
    for (int i = 0; i < pInstance->m; i++) {
        sum += counts[i];
    }
    sum /= 2;
    cout << sum << endl;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < pInstance->m; i++) {
        for (int j = i + 1; j < pInstance->m; j++) {
            int intersect = Edge::intersect(&edges->at(i), &edges->at(j));
            if (intersect) {
                counts[i] += 1;
                counts[j] += 1;
            }
            cache[i][j] = intersect;
        }
    }

    sum = 0;
    for (int i = 0; i < pInstance->m; i++) {
        sum += counts[i];
    }
    sum /= 2;
    cout << sum << endl;

    cout << "Intersection cache filled." << endl;

    // Since we cant atomically push_back we do it like this instead.
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < pInstance->m; i++) {
        for (int j = 0; j < pInstance->m; j++) {
            if (intersects(i, j)) {
                adjacency_list[i]->push_back(j);
            }
        }
    }

    Cache::initialized = true;
    cout << "Adjacency cache filled." << endl;
}

bool Cache::intersects(int i, int j) {
    if (i > j) return cache[j][i];
    return cache[i][j];
}

int Cache::get_count(int i) {
    return counts[i];
}

vector<int> *Cache::neighbours(int i) {
    return adjacency_list[i];
}