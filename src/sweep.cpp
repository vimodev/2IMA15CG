#include "sweep.h"

#include <queue>
#include <map>
#include <algorithm>

#include "cache.h"

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

static void insert_endpoints(priority_queue<Event> *Q) {
    for (int i = 0; i < S->size(); i++) {
        Vertex *v1 = S->at(i).v1;
        Vertex *v2 = S->at(i).v2;
        // v1 is upper endpoint
        if (v1->y > v2->y) {
            Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, UPPER);
            // keys[i] = {(long double) 1.0 * v1->x, (long double) v1->y};
            Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, LOWER);
        // v2 is upper endpoint
        } else if (v1->y < v2->y) {
            Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, UPPER);
            // keys[i] = {(long double) 1.0 * v2->x, (long double) v2->y};
            Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, LOWER);
        // Left endpoint is upper on equality
        } else if (v1->y == v2->y) {
            if (v1->x < v2->x) {
                Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, UPPER);
                // keys[i] = {(long double) 1.0 * v1->x, (long double) v1->y};
                Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, LOWER);
            } else {
                Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, i, -1, UPPER);
                // keys[i] = {(long double) 1.0 * v2->x, (long double) v2->y};
                Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, i, -1, LOWER);
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

// Was p1 handled before p2?
static bool before(Point p1, Point p2) {
    if (p1.y > p2.y) return true;
    if (p1.y < p2.y) return false;
    if (p1.x < p2.x) return true;
    return false;
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

static StatusIter getIterator(multimap<long double, int> *T, long double x, int edge) {
    auto range = T->equal_range(x);
    for (auto iter = range.first; iter != range.second; ++iter) {
        if (iter->second == edge) {
            return iter;
        }
    }
    return T->end();
}

static pair<StatusIter, StatusIter> getLeft(multimap<long double, int> *T, StatusIter iter) {
    auto left = T->lower_bound(iter->first);
    if (left == T->begin()) return {T->end(), T->end()};
    return T->equal_range(prev(left)->first);
}

static pair<StatusIter, StatusIter> getRight(multimap<long double, int> *T, StatusIter iter) {
    auto right = T->upper_bound(iter->first);
    if (right == T->end()) return {T->end(), T->end()};
    return T->equal_range(right->first);
}

static long double getKey(int edge) {
    return keys[edge];
    // Vertex *v1 = edge->v1;
    // Vertex *v2 = edge->v2;
    // if (v1->y > v2->y) return (long double) 1.0 * v1->x;
    // if (v1->y < v2->y) return (long double) 1.0 * v2->x;
    // if (v1->x < v2->x) return (long double) 1.0 * v1->x;
    // return (long double) 1.0 * v2->x;
}

static void handle_event(Event e, priority_queue<Event> *Q, multimap<long double, int> *T) {
    if (e.type == UPPER) {
        // cout << "Handling upper endpoint (" << e.p.x << "," << e.p.y << ") of edge " << e.e1 << endl;
        Edge *edge = &S->at(e.e1);
        // Insert the segment s at point p
        T->insert({e.p.x, e.e1});
        keys[e.e1] = e.p.x;
        // Get its location
        auto location = getIterator(T, e.p.x, e.e1);
        // Check left for collision
        auto left_range = getLeft(T, location);
        // Go over all equal neighbours
        for (auto iter = left_range.first; iter != left_range.second; iter++) {
            Edge *other = &S->at(iter->second);
            // If other is above or left<= the intersection will have been noted already
            // if (before(getUpper(*other), getUpper(*edge))) continue;
            // If they intersect
            if (Edge::intersect(edge, other)) {
                // Add the event
                Point intersection = intersection_point(e.e1, iter->second);
                Q->emplace(intersection.x, intersection.y, iter->second, e.e1, INTERSECT);
                // cout << iter->second << " " << e.e1 << " added to queue" << endl;
            }
        }
        // Check right for collision
        auto right_range = getRight(T, location);
        // Go over all neighbours >
        for (auto iter = right_range.first; iter != right_range.second; iter++) {
            Edge *other = &S->at(iter->second);
            // if (before(getUpper(*other), getUpper(*edge))) continue;
            // If they intersect
            if (Edge::intersect(edge, other)) {
                // Add the event
                Point intersection = intersection_point(e.e1, iter->second);
                Q->emplace(intersection.x, intersection.y, e.e1, iter->second, INTERSECT);
                // cout << e.e1 << " " << iter->second << " added to queue" << endl;
            }
        }
    } else if (e.type == LOWER) {
        // cout << "Handling lower endpoint (" << e.p.x << "," << e.p.y << ") of edge " << e.e1 << endl;
        Edge *edge = &S->at(e.e1);
        // Find the x of the other endpoint
        long double key = getKey(e.e1);
        // Get its location
        auto location = getIterator(T, key, e.e1);
        // Get left and right neighbours
        auto left_range = getLeft(T, location);
        auto right_range = getRight(T, location);
        // Check for pairwise intersection
        for (auto left = left_range.first; left != left_range.second; left++) {
            for (auto right = right_range.first; right != right_range.second; right++) {
                // If they intersect
                if (Edge::intersect(&S->at(left->second), &S->at(right->second))) {
                    // Calculate the point and check if its below sweepline
                    Point intersection = intersection_point(left->second, right->second);
                    if (intersection.y < e.p.y) {
                        // If its below sweepline add it
                        Q->emplace(intersection.x, intersection.y, left->second, right->second, INTERSECT);
                        // cout << left->second << " " << right->second << " added to queue" << endl;
                    }
                }
            }
        }
    } else if (e.type == INTERSECT) {
        if (Cache::intersects(e.e1, e.e2)) return;
        // Get left info
        long double left_key = getKey(e.e1);
        auto left_loc = getIterator(T, left_key, e.e1);
        int left_edge = left_loc->second;
        T->erase(left_loc);
        // Get right info
        long double right_key = getKey(e.e2);
        auto right_loc = getIterator(T, right_key, e.e2);
        int right_edge = right_loc->second;
        T->erase(right_loc);
        // Swap
        T->insert({left_key, right_edge});
        keys[right_edge] = left_key;
        T->insert({right_key, left_edge});
        keys[left_edge] = right_key;
        // Check if left now intersects
        left_loc = getIterator(T, left_key, right_edge);
        auto left_range = getLeft(T, left_loc);
        // Go over all equal neighbours
        for (auto iter = left_range.first; iter != left_range.second; iter++) {
            Edge *other = &S->at(iter->second);
            // If they intersect
            if (Edge::intersect(&S->at(right_edge), other)) {
                // Add the event if they intersect below
                Point intersection = intersection_point(right_edge, iter->second);
                if (intersection.y < e.p.y) {
                    Q->emplace(intersection.x, intersection.y, iter->second, right_edge, INTERSECT);
                    // cout << iter->second << " " << right_edge << " added to queue" << endl;
                }
            }
        }
        // Check if right now intersects
        right_loc = getIterator(T, right_key, left_edge);
        auto right_range = getRight(T, right_loc);
        // Go over all equal neighbours
        for (auto iter = right_range.first; iter != right_range.second; iter++) {
            Edge *other = &S->at(iter->second);
            // If they intersect
            if (Edge::intersect(&S->at(left_edge), other)) {
                // Add the event if they intersect below
                Point intersection = intersection_point(left_edge, iter->second);
                if (intersection.y < e.p.y) {
                    Q->emplace(intersection.x, intersection.y, left_edge, iter->second, INTERSECT);
                    // cout << left_edge << " " << iter->second << " added to queue" << endl;
                }
            }
        }
        int i;
        int j;
        if (left_edge <= right_edge) {
            i = left_edge;
            j = right_edge;
        } else {
            i = right_edge;
            j = left_edge;
        }
        // cout << i << " " << j << " intersect at (" << e.p.x << "," << e.p.y << ")" << endl;
        Cache::cache[i][j] = true;
        Cache::counts[i]++;
        Cache::counts[j]++;
    }

}

void Sweepline::sweep(vector<Edge> *set) {
    S = set;
    keys.clear();
    keys.resize(set->size());
    priority_queue<Event> Q;
    multimap<long double, int> T;
    insert_endpoints(&Q);
    while (!Q.empty()) {
        Event e = Q.top();
        Q.pop();
        handle_event(e, &Q, &T);
    }
}