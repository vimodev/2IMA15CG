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
// static priority_queue<Event> Q;
static set<Event> Q;
bitset<MAX_EDGES> queued[MAX_EDGES];


string toString(EventType t) {
    if (t == UPPER) return "upper";
    if (t == LOWER) return "lower";
    return "intersect";
}

void printStatus() {
    cout << "STATUS: ";
    for (auto p : T) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;
}

void printQueue() {
    cout << "QUEUE: ";
    for (auto p : Q) {
        cout << "(" << toString(p.type) << ", " << p.p.x << " " << p.p.y << ", " << p.e1 << ") ";
    }
    cout << endl;
}

void setQueued(int i, int j) {
    if (i > j) setQueued(j, i);
    queued[i][j] = true;
}

bool getQueued(int i, int j) {
    if (i > j) return getQueued(j, i);
    return queued[i][j];
}

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
    if (D) {
        long double x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / D;
        long double y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / D;
        return {x, y};
    } else {
        // Check x coords
        if (std::max(x1,x2) == std::max(x3,x4) && std::min(x1,x2) == std::min(x3,x4)) {
            if (x1 <= std::max(x3,x4) && x1 >= std::min(x3,x4)) {
                return {x1, y1};
            }
            if (x2 <= std::max(x3,x4) && x2 >= std::min(x3,x4)) {
                return {x2, y2};
            }
            if (x3 <= std::max(x1,x2) && x3 >= std::min(x1,x2)) {
                return {x3, y3};
            }
            if (x4 <= std::max(x1,x2) && x4 >= std::min(x1,x2)) {
                return {x4, y4};
            }
        }

        // Check y coords
        if (std::max(y1,y2) == std::max(y3,y4) && std::min(y1,y2) == std::min(y3,y4)) {
            if (y1 <= std::max(y3,y4) && y1 >= std::min(y3,y4)) {
                return {x1, y1};
            }
            if (y2 <= std::max(y3,y4) && y2 >= std::min(y3,y4)) {
                return {x2, y2};
            }
            if (y3 <= std::max(y1,y2) && y3 >= std::min(y1,y2)) {
                return {x3, y3};
            }
            if (y4 <= std::max(y1,y2) && y4 >= std::min(y1,y2)) {
                return {x4, y4};
            }
        }
    }
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

static StatusIter addStatus(long double key, int edge) {
    keys[edge] = key;
    return T.insert({key, edge});
}

void addBounds() {
    int min_x = INT_MAX;
    int min_y = INT_MAX;
    int max_x = INT_MIN;
    int max_y = INT_MIN;
    for (Edge e : *S) {
        auto v1 = e.v1;
        auto v2 = e.v2;
        if (v1->x < min_x) min_x = v1->x;
        if (v1->x > max_x) max_x = v1->x;
        if (v2->x < min_x) min_x = v2->x;
        if (v2->x > max_x) max_x = v2->x;
        if (v1->y < min_y) min_y = v1->y;
        if (v1->y > max_y) max_y = v1->y;
        if (v2->y < min_y) min_y = v2->y;
        if (v2->y > max_y) max_y = v2->y;
    }
    Vertex *top_left = new Vertex(min_x - 1, max_y + 1);
    Vertex *top_right = new Vertex(max_x + 1, max_y + 1);
    Vertex *bottom_left = new Vertex(min_x - 1, min_y - 1);
    Vertex *bottom_right = new Vertex(max_x + 1, min_y - 1);
    S->emplace_back(top_left, bottom_left);
    S->emplace_back(top_right, bottom_right);
    T.insert({(long double) min_x - 1, S->size() - 2});
    T.insert({(long double) max_x + 1, S->size() - 1});
}

void removeBounds() {
    S->pop_back();
    S->pop_back();
}

pair<StatusIter, StatusIter> getLeftRange(StatusIter iter) {
    auto lower = T.lower_bound(iter->first);
    return {T.lower_bound(prev(lower)->first), T.upper_bound(iter->first)};
}

pair<StatusIter, StatusIter> getRightRange(StatusIter iter) {
    auto upper = T.upper_bound(iter->first);
    return {T.lower_bound(iter->first), T.upper_bound(upper->first)};
}

void handleUpper(Event e) {
    if (DEBUG) cout << "Upper " << e.e1 << endl;
    auto loc = addStatus(e.p.x, e.e1);
    auto range = getLeftRange(loc);
    for (auto other = range.first; other != range.second; other++) {
        if (loc->second == other->second) continue;
        if (!getQueued(loc->second, other->second) && Edge::intersect(&S->at(loc->second), &S->at(other->second))) {
            Point p = intersection_point(loc->second, other->second);
            Q.emplace(p.x, p.y, loc->second, other->second, INTERSECTION);
            setQueued(loc->second, other->second);
        }
    }
    range = getRightRange(loc);
    for (auto other = range.first; other != range.second; other++) {
        if (loc->second == other->second) continue;
        if (!getQueued(loc->second, other->second) && Edge::intersect(&S->at(loc->second), &S->at(other->second))) {
            Point p = intersection_point(loc->second, other->second);
            Q.emplace(p.x, p.y, loc->second, other->second, INTERSECTION);
            setQueued(loc->second, other->second);
        }
    }
}

void handleLower(Event e) {
    if (DEBUG) cout << "Lower " << e.e1 << endl;
    auto loc = getIterator(keys[e.e1], e.e1);
    auto left = getLeftRange(loc);
    auto right = getRightRange(loc);
    if (right.first->second == e.e1) right.first = next(right.first);
    T.erase(loc);
    for (auto l = left.first; l != left.second; l++) {
        for (auto r = right.first; r != right.second; r++) {
            if (l->second == r->second) continue;
            if (!getQueued(l->second, r->second) && Edge::intersect(&S->at(l->second), &S->at(r->second))) {
                Point p = intersection_point(l->second, r->second);
                if (p.y <= e.p.y) {
                    Q.emplace(p.x, p.y, l->second, r->second, INTERSECTION);
                    setQueued(l->second, r->second);
                }
            }
        }
    }
}

pair<StatusIter, StatusIter> getRange(int e1, int e2) {
    auto i1 = getIterator(keys[e1], e1);
    auto i2 = getIterator(keys[e2], e2);
    if (distance(i1, T.end()) > distance(i2, T.end())) return {i1, i2};
    return {i2, i1};
}

pair<StatusIter, StatusIter> swapSegments(int e1, int e2) {
    auto i1 = getIterator(keys[e1], e1);
    auto i2 = getIterator(keys[e2], e2);
    i1->second = e2;
    keys[e2] = i1->first;
    i2->second = e1;
    keys[e1] = i2->first;
    return getRange(e1, e2);
}

void handleIntersection(Event e) {
    if (Cache::intersects(e.e1, e.e2)) return;
    if (DEBUG) cout << "Intersection " << e.e1 << " " << e.e2 << endl;
    auto swapped = swapSegments(e.e1, e.e2);
    auto left = getLeftRange(swapped.first);
    auto right = getRightRange(swapped.second);
    for (auto i1 = left.first; i1 != right.second; i1++) {
        for (auto i2 = left.first; i2 != right.second; i2++) {
            if (i1->second == i2->second) continue;
            if (!getQueued(i1->second, i2->second) && Edge::intersect(&S->at(i1->second), &S->at(i2->second))) {
                Point p = intersection_point(i1->second, i2->second);
                if (p.y <= e.p.y) {
                    Q.emplace(p.x, p.y, i1->second, i2->second, INTERSECTION);
                    setQueued(i1->second, i2->second);
                }
            }
        }
    }
    Cache::setIntersect(e.e1, e.e2);
}

void Sweepline::sweep(vector<Edge> *set) {
    S = set;
    keys.clear();
    keys.resize(set->size());
    insert_endpoints();
    addBounds();
    int sum = 0;
    while (!Q.empty()) {
        Event e = *Q.begin();
        Q.erase(Q.begin());
        if (e.type == UPPER)        handleUpper(e);
        if (e.type == LOWER)        handleLower(e);
        if (e.type == INTERSECTION) handleIntersection(e);
        // if (Q.size() % 1000 == 0) cout << Q.size() << endl;
        // cout << T.size() << endl;
        if (sum % 10000 == 0) {
            auto y = e.p.y;
            long double starty = S->at(S->size() - 1).v1->y;
            long double endy = S->at(S->size() - 1).v2->y;
            long double perc = (starty - y) / (starty - endy) * 100.0;
            cout << "|Q|=" << Q.size() << " |T|=" << T.size() << " %=" << perc << endl;
        } 
        if (DEBUG) printStatus();
        sum++;
    }
    cout << "#events: " << sum << endl;
    removeBounds();
}