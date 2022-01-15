#include "sweep.h"
#include "cache.h"

#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <limits>

#define DEBUG false

struct CompareVertex {
    bool operator()(Vertex const& v1, Vertex const& v2) {
        return (v1.x > v2.x || (v1.x == v2.x && v1.y >= v2.y));
    }
};

// A robust sweepline for finding intersections of line segments
// Following https://www.researchgate.net/publication/267800773_A_Robust_and_EEcient_Implementation_of_a_Sweep_Line_Algorithm_for_the_Straight_Line_Segment_Intersection_Problem
// Maxim Snoep
void Sweepline::sweep(vector<Edge> *set) {
    
    // Local declarations
    // TODO:
    priority_queue<Vertex, vector<Vertex>, CompareVertex> X_structure;
    priority_queue< pair<Edge, int> > Y_structure;
    list<Edge> internal;
    priority_queue< pair<Vertex, Edge> > seg_queue;
    int infinity = 0;

    // Initialization
    // TODO:
    for (Edge e : *set) {
        X_structure.push(*(e.v1));
        X_structure.push(*(e.v2));
        infinity = max(infinity, abs(e.v1->x));
        infinity = max(infinity, abs(e.v1->y));
        infinity = max(infinity, abs(e.v2->x));
        infinity = max(infinity, abs(e.v2->y));
    }
    // Add the sentinel start and stop points
    X_structure.push(Vertex(-infinity, -infinity));
    X_structure.push(Vertex(infinity, infinity));

    // Sweep
    // TODO:
    while (!X_structure.empty()) {
        Vertex v = X_structure.top();
        X_structure.pop();
        cout << v.x << ", " << v.y << endl;
    }


}