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

    cout << "[INFO] Starting preparation." << endl;

    Sweepline::sweep(edges);
    int sum = 0;
    for (int i = 0; i < pInstance->m; i++) {
        for (int j = i + 1; j < pInstance->m; j++) {
            if (cache[i][j]) sum++;
        }
    }
    cout << "\n  -- INTERSECTION  SUMMARY --  " << endl;
    cout << "        Found    : " << sum << endl;

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < pInstance->m; i++) {
        for (int j = i + 1; j < pInstance->m; j++) {
            int intersect = Edge::intersect(&edges->at(i), &edges->at(j));
            if (!intersect && cache[i][j]) {
                cout << "Falsely " << i << ", " << j << endl;
            }
            if (cache[i][j]) continue;
            if (intersect) {
                // cout << i << " " << j << endl;
                counts[i] += 1;
                counts[j] += 1;
            }
            cache[i][j] = intersect;
        }
    }
    
    int sum2 = 0;
    for (int i = 0; i < pInstance->m; i++) {
        for (int j = i + 1; j < pInstance->m; j++) {
            if (cache[i][j]) sum2++;
        }
    }
    cout << "        Actual   : " << sum2 << endl;
    cout << "        Accuracy : " << (sum*100/sum2) << "%" << endl;
    cout << "  - - - - - - - - - - - - - -  \n" << endl;


    cout << "[INFO] Intersection cache filled." << endl;

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
    cout << "[INFO] Adjacency cache filled." << endl;
}
void Cache::setIntersect(int i, int j) {
    if (i > j) setIntersect(j, i);
    cache[i][j] = true;
    counts[i]++;
    counts[j]++;
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