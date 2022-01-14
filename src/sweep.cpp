#include "sweep.h"

#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <limits>

#include "cache.h"

#define DEBUG false

typedef std::multimap<long double, int>::iterator StatusIter;

Point::Point(long double x, long double y) {
    this->x = x; this->y = y;
}

Point::Point() {
    this->x = 0.0; this->y = 0.0;
}

Event::Event(long double x, long double y, int e1, int e2, EventType type) {
    this->p.x = x; this->p.y = y;
    this->e1 = e1; this->e2 = e2;
    this->type = type;
}

static vector<Edge> *S;
static vector<long double> keys;
static multimap<long double, int> T;
static priority_queue<Event> Q;

static void insert_endpoints() {
    for (int i = 0; i < S->size(); i++) {
        Vertex *v1 = S->at(i).v1;
        Vertex *v2 = S->at(i).v2;
        // v1 is upper endpoint
        if (v1->y > v2->y) {
            Q.emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, UPPER);
            // keys[i] = {(long double) 1.0 * v1->x, (long double) v1->y};
            Q.emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, LOWER);
        // v2 is upper endpoint
        } else if (v1->y < v2->y) {
            Q.emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, UPPER);
            // keys[i] = {(long double) 1.0 * v2->x, (long double) v2->y};
            Q.emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, LOWER);
        // Left endpoint is upper on equality
        } else if (v1->y == v2->y) {
            if (v1->x < v2->x) {
                Q.emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, UPPER);
                // keys[i] = {(long double) 1.0 * v1->x, (long double) v1->y};
                Q.emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, LOWER);
            } else {
                Q.emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, UPPER);
                // keys[i] = {(long double) 1.0 * v2->x, (long double) v2->y};
                Q.emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, LOWER);
            }
        }
    }
}

static Point getUpper(Edge e) {
    Vertex *v1 = e.v1;
    Vertex *v2 = e.v2;
    if (v1->y > v2->y) return {(long double)v1->x, (long double)v1->y};
    if (v1->y < v2->y) return {(long double)v2->x, (long double)v2->y};
    if (v1->x < v2->x) return {(long double)v1->x, (long double)v1->y};
    return {(long double)v2->x, (long double)v2->y};
}

Point intersection_point(int i1, int i2) {
    auto e1 = S->at(i1); auto e2 = S->at(i2);
    long double x1 = e1.v1->x; long double y1 = e1.v1->y;
    long double x2 = e1.v2->x; long double y2 = e1.v2->y;
    long double x3 = e2.v1->x; long double y3 = e2.v1->y;
    long double x4 = e2.v2->x; long double y4 = e2.v2->y;
    long double D = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    long double x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / D;
    long double y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / D;
    return {x, y};
}

static StatusIter getIterator(long double x, int edge) {
    auto range = T.equal_range(x);
    for (auto iter = range.first; iter != range.second; ++iter) {
        if (iter->second == edge) {
            return iter;
        }
    }
    return T.end();
}

// Get everything to the left of the iterator, including equal values
static pair<StatusIter, StatusIter> getLeft(StatusIter iter) {
    // Range of values equal to iterator
    auto equal = T.equal_range(iter->first);
    // We add the rightmost of those as the right limit
    auto right = equal.second;
    // Left limit is the leftmost of those
    auto left = equal.first;
    // But if there are more to the left
    if (equal.first != T.begin()) {
        // Add 1 step more
        left = T.lower_bound(prev(equal.first)->first);
    }
    return {left, right};
}

// Get everything directly right of iterator
static pair<StatusIter, StatusIter> getRight(StatusIter iter) {
    // Equal values
    auto equal = T.equal_range(iter->first);
    // Set bounds
    auto left = equal.first;
    auto right = equal.second;
    // If there is place to the right
    if (right != T.end()) {
        // Get the direct right element
        // So get upper bound, and all values equal to that, and then get the right most
        right = T.equal_range(right->first).second;
    }
    return {left, right};
}

static pair<StatusIter, StatusIter> getArea(long double x) {
    auto equal = T.equal_range(x);
    if (equal.first == T.end()) return {T.end(), T.end()};
    // There were equal values
    if (equal.first->first == x) {
        auto range = equal;
        // If there is something to the left
        if (equal.first != T.begin()) {
            range.first = T.lower_bound(prev(equal.first)->first);
        }
        // If there is something to the right
        if (equal.second != T.end()) {
            range.second = T.upper_bound(equal.second->first);
        }
        return range;
    // There were no equal values
    } else {
        auto range = equal;
        // If there is something to the left
        if (equal.first != T.begin()) {
            range.first = T.lower_bound(prev(equal.first)->first);
        }
        // If there is something to the right
        if (equal.second != T.end()) {
            range.second = T.upper_bound(equal.second->first);
        }
        return range;
    }
}

static long double getKey(int edge) {
    return keys[edge];
}

static StatusIter addStatus(long double key, int edge) {
    keys[edge] = key;
    return T.insert({key, edge});
}

void printStatus() {
    cout << "STATUS: ";
    for (auto p : T) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;
}

string toString(EventType t) {
    if (t == UPPER) return "upper";
    if (t == LOWER) return "lower";
    return "intersect";
}

// Handle an upper endpoint event
static void handleUpper(Event e) {
    long double x = e.p.x;
    int edge = e.e1;
    if (DEBUG) cout << "Adding (" << x << "," << edge << ")" << endl;
    auto location = addStatus(x, edge);
    // Check left for intersection
    auto left_range = getLeft(location);
    for (auto iter = left_range.first; iter != left_range.second; iter++) {
        if (iter->second == edge) continue;
        if (DEBUG) cout << "Checking " << edge << " " << iter->second << endl;
        if (Edge::intersect(&S->at(edge), &S->at(iter->second))) {
            Point p = intersection_point(edge, iter->second);
            Q.emplace(p.x, p.y, edge, iter->second, INTERSECT);
        }
    }
    // Check right for itnersection
    auto right_range = getRight(location);
    for (auto iter = right_range.first; iter != right_range.second; iter++) {
        if (iter->second == edge) continue;
        if (DEBUG) cout << "Checking " << edge << " " << iter->second << endl;
        if (Edge::intersect(&S->at(edge), &S->at(iter->second))) {
            Point p = intersection_point(edge, iter->second);
            Q.emplace(p.x, p.y, edge, iter->second, INTERSECT);
        }
    }
}

// Handle a lower endpoint event
static void handleLower(Event e) {
    int edge = e.e1;
    long double x = keys[edge];
    if (DEBUG) cout << "Removing (" << x << "," << edge << ")" << endl;
    // Remove the edge from status
    T.erase(getIterator(x, edge));
    // Get the direct area
    auto range = getArea(x);
    // if (DEBUG) cout << range.first->second << " " << prev(range.second)->second << endl;
    // Loop over the entire area to check for pairwise intersection
    for (auto i1 = range.first; i1 != range.second; i1++) {
        for (auto i2 = range.first; i2 != range.second; i2++) {
            if (i1->first == i2->first) continue;
            if (Edge::intersect(&S->at(i1->second), &S->at(i2->second))) {
                Point p = intersection_point(i1->second, i2->second);
                if (p.y <= e.p.y && !Cache::intersects(i1->second, i2->second)) {
                    Q.emplace(p.x, p.y, i1->second, i2->second, INTERSECT);
                }
            }
        }
    }
}

static void handleIntersect(Event e) {
    if (Cache::intersects(e.e1, e.e2)) return;
    // Make sure e1 is left and e2 is right
    if (keys[e.e1] > keys[e.e2]) {
        auto temp = e.e1;
        e.e1 = e.e2;
        e.e2 = temp;
    }
    if (DEBUG) cout << "INTERSECTION: " << e.e1 << " " << e.e2 << endl;
    if (DEBUG) cout << "Swapping (" << keys[e.e1] << "," << e.e1 << ") and (" << keys[e.e2] << "," << e.e2 << ")" << endl;
    Cache::setIntersect(e.e1, e.e2);
    // Swap the elements
    auto left = getIterator(keys[e.e1], e.e1);
    auto right = getIterator(keys[e.e2], e.e2);
    left->second = e.e2;
    keys[e.e2] = left->first;
    right->second = e.e1;
    keys[e.e1] = right->first;
    // Check for intersections in the area again
    auto left_range = getArea(left->first);
    for (auto iter = left_range.first; iter != left_range.second; iter++) {
        if (iter->second == left->second) continue;
        if (Edge::intersect(&S->at(left->second), &S->at(iter->second))) {
            Point p = intersection_point(left->second, iter->second);
            if (p.y <= e.p.y && !Cache::intersects(left->second, iter->second)) {
                Q.emplace(p.x, p.y, left->second, iter->second, INTERSECT);
            }
        }
    }
    auto right_range = getArea(right->first);
    for (auto iter = right_range.first; iter != right_range.second; iter++) {
        if (iter->second == right->second) continue;
        if (Edge::intersect(&S->at(right->second), &S->at(iter->second))) {
            Point p = intersection_point(right->second, iter->second);
            if (p.y <= e.p.y && !Cache::intersects(right->second, iter->second)) {
                Q.emplace(p.x, p.y, right->second, iter->second, INTERSECT);
            }
        }
    }
}

void Sweepline::sweep(vector<Edge> *set) {
    S = set;
    keys.clear();
    keys.resize(set->size());
    insert_endpoints();
    while (!Q.empty()) {
        if (DEBUG) printStatus();
        Event e = Q.top();
        if (e.type == UPPER) handleUpper(e);
        if (e.type == LOWER) handleLower(e);
        if (e.type == INTERSECT) handleIntersect(e);
        Q.pop();
    }
}