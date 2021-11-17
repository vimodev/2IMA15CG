#include "adjacency_list.h"

bool AdjacencyList::initialized = false;
vector<int> *AdjacencyList::adjacency_list[MAX_EDGES];

// Initialize the adjacency list
void AdjacencyList::initialize() {
    if (initialized) {
        cout << "Can not initialize adjacency list twice" << endl;
        return;
    }
    if (!IntersectionCache::initialized) {
        cout << "Intersection cache must be initialized before initializing the adjacency list" << endl;
        exit(EXIT_FAILURE);
    }
    // Initialize vectors at each index
    for (int i = 0; i < IntersectionCache::instance->m; i++) {
        adjacency_list[i] = new vector<int>;
    }
    populate();
    AdjacencyList::initialized = true;
    cout << "Adjacency list filled." << endl;
}

void AdjacencyList::populate() {
    for (int i = 0; i < IntersectionCache::instance->m; i++) {
        for (int j = i + 1; j < IntersectionCache::instance->m; j++) {
            if (IntersectionCache::get(i, j)) {
                adjacency_list[i]->push_back(j);
                adjacency_list[j]->push_back(i);
            }
        }
    }
}

bool AdjacencyList::edge_exists(int i, int j) {
    return (find(adjacency_list[i]->begin(), adjacency_list[i]->end(), j) != adjacency_list[i]->end());
}

vector<int> *AdjacencyList::neighbours(int i) {
    return adjacency_list[i];
}