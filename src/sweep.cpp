#include "sweep.h"
#include "cache.h"

#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <limits>

#include "cache.h"

#define DEBUG true

typedef std::vector<int>::iterator StatusIter;

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

long double get_x_on_e_with_y(Edge e, long double y) {
    if (e.v1->y == e.v2->y) {
        return e.v1->x;
    } else {
        return e.v1->x + (e.v2->x - e.v1->x)*((y - e.v1->y)/(e.v2->y - e.v1->y));
    }
}

static vector<Edge> *S;
static vector<int> T;
// static priority_queue<Event> Q;
static set<Event> Q;
bitset<MAX_EDGES> queued[MAX_EDGES];


string toString(EventType t) {
    if (t == UPPER) return "upper";
    if (t == LOWER) return "lower";
    return "intersect";
}

void printStatus(double long y) {
    cout << "STATUS at y:" << y << " -> ";
    for (int e : T) {
        cout << "[ edge" << e << " (" << get_x_on_e_with_y(S->at(e), y) << ") ]";
    }
    cout << endl;
};

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

static StatusIter getIteratorRec(long double x, long double y, int e, int l, int r) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (T[mid] == e) {
            return T.begin() + mid;
        }
        Edge mid_e = S->at(T[mid]);
        int mid_x = get_x_on_e_with_y(mid_e, y);
        if (mid_x > x) {
            return getIteratorRec(x, y, e, l, mid - 1);
        } else {
            return getIteratorRec(x, y, e, mid + 1, r);
        }
    }
    return T.end();
}

static StatusIter getIterator(long double y, int e) {
    Edge edge = S->at(e);
    long double x = get_x_on_e_with_y(edge, y);
    return getIteratorRec(x, y, e, 0, T.size());
}

static StatusIter addStatusRec(long double x, long double y, int e, int l, int r) {
    cout << "adding with x:" << x << " y:" << y << " the edge:" << e << "... between l:" << l << " and r:" << r << endl;
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (mid >= T.size()) {
            cout << "inserting edge: " << e << " at " << mid << endl;
            T.insert(T.begin() + mid, e);
            return T.begin() + mid;
        }
        Edge mid_e = S->at(T[mid]);
        int mid_x = get_x_on_e_with_y(mid_e, y);
        cout << "mid at " << mid << " has x: " << mid_x << endl;
        if (mid_x == x) {
            cout << "inserting edge: " << e << " at " << mid << endl;
            T.insert(T.begin() + mid, e);
            return T.begin() + mid;
        }
        if (mid_x > x) {
            return addStatusRec(x, y, e, l, mid - 1);
        } else {
            return addStatusRec(x, y, e, mid + 1, r);
        }
    }
    cout << "inserting edge: " << e << " at " << l << endl;
    T.insert(T.begin() + l, e);
    return T.begin() + l;
}

static StatusIter addStatus(long double y, int e) {
    Edge edge = S->at(e);
    long double x = get_x_on_e_with_y(edge, y);
    if (T.size() == 0) {
        T.push_back(e);
        return T.begin();
    }
    return addStatusRec(x, y, e, 0, T.size());
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
    cout << S->size()-2 << " " << S->size()-1 << endl;
    addStatus(0, S->size()-2);
    addStatus(0, S->size()-1);
}

void removeBounds() {
    S->pop_back();
    S->pop_back();
}

pair<StatusIter, StatusIter> getLeftRange(StatusIter iter, long double y) {
    int x = get_x_on_e_with_y(S->at(*(iter-1)), y);

    StatusIter it1 = iter;
    for (; it1 != T.begin(); --it1) {
        if (get_x_on_e_with_y(S->at(*it1), y) < x-1) break;
    }

    StatusIter it2 = iter;
    for (; it2 != T.end(); ++it2) {
        if (get_x_on_e_with_y(S->at(*it2), y) > x) break;
    }


    return {it1, it2};
}

pair<StatusIter, StatusIter> getRightRange(StatusIter iter, long double y) {
    int x = get_x_on_e_with_y(S->at(*(iter+1)), y);

    StatusIter it1 = iter;
    for (; it1 != T.begin(); --it1) {
        if (get_x_on_e_with_y(S->at(*it1), y) < x) break;
    }

    StatusIter it2 = iter;
    for (; it2 != T.end(); ++it2) {
        if (get_x_on_e_with_y(S->at(*it2), y) > x+1) break;
    }


    return {it1, it2};
}

void handleUpper(Event e) {
    if (DEBUG) cout << "[DEBUG] Upper endpoint of edge:" << e.e1 << " at (" << e.p.x << ", " << e.p.y << ")" << endl;
    auto loc = addStatus(e.p.y, e.e1);
    auto range = getLeftRange(loc, e.p.y);
    cout << *range.first << " - " << *range.second << endl;
    for (auto other = range.first; other != range.second; other++) {
        cout << *other << " loc:" << *loc << endl;
        if (loc == other) {
            continue;
        }
        if (!getQueued(*loc, *other) && Edge::intersect(&S->at(*loc), &S->at(*other))) {
            Point p = intersection_point(*loc, *other);
            Q.emplace(p.x, p.y, *loc, *other, INTERSECTION);
            setQueued(*loc, *other);
        }
    }
    range = getRightRange(loc, e.p.y);
    cout << *range.first << " - " << *range.second << endl;
    for (auto other = range.first; other != range.second; other++) {
        cout << *other << " loc:" << *loc << endl;
        if (loc == other) {
            continue;
        }
        if (!getQueued(*loc, *other) && Edge::intersect(&S->at(*loc), &S->at(*other))) {
            Point p = intersection_point(*loc, *other);
            Q.emplace(p.x, p.y, *loc, *other, INTERSECTION);
            setQueued(*loc, *other);
        }
    }
}

void handleLower(Event e) {
    if (DEBUG) cout << "[DEBUG] Lower endpoint of edge: " << e.e1 << " at (" << e.p.x << ", " << e.p.y << ")" << endl;
    auto loc = find(T.begin(), T.end(), e.e1);
    auto left = getLeftRange(loc, e.p.y);
    auto right = getRightRange(loc, e.p.y);
    if (*(right.first) == e.e1) right.first = next(right.first);
    T.erase(loc);
    for (auto l = left.first; l != left.second; l++) {
        for (auto r = right.first; r != right.second; r++) {
            if (*l == *r) continue;
            if (!getQueued(*l, *r) && Edge::intersect(&S->at(*l), &S->at(*r))) {
                Point p = intersection_point(*l, *r);
                if (p.y <= e.p.y) {
                    Q.emplace(p.x, p.y, *l, *r, INTERSECTION);
                    setQueued(*l, *r);
                }
            }
        }
    }
}

pair<StatusIter, StatusIter> getRange(int e1, int e2, long double cur_y) {
    auto i1 = getIterator(cur_y, e1);
    auto i2 = getIterator(cur_y, e2);
    // if (distance(i1, T.end()) > distance(i2, T.end())) return {i1, i2};
    return {i2, i1};
}

pair<StatusIter, StatusIter> swapSegments(int e1, int e2, long double cur_y) {
    auto i1 = getIterator(cur_y, e1);
    auto i2 = getIterator(cur_y, e2);
    iter_swap(i1, i2);
    return getRange(e1, e2, cur_y);
}

void handleIntersection(Event e) {
    if (Cache::intersects(e.e1, e.e2)) return;
    if (DEBUG) cout << "[DEBUG] Intersection between edge:" << e.e1 << " and edge:" << e.e2 << " at (" << e.p.x << ", " << e.p.y << ")" << endl;
    auto swapped = swapSegments(e.e1, e.e2, e.p.y);
    cout << *(swapped.first) << " " << *(swapped.second) << endl;
    auto left = getLeftRange(swapped.first, e.p.y);
    auto right = getRightRange(swapped.second, e.p.y);
    cout << "left: " << *left.first << " - " << *left.second << endl;
    cout << "right: " << *right.first << " - " << *right.second << endl;

    for (auto i1 = T.begin(); i1 != T.end(); i1++) {
        cout << *i1 << endl;
    }

    for (auto i1 = left.first; i1 != right.second; i1++) {
        for (auto i2 = left.first; i2 != right.second; i2++) {
            cout << *i1 << " " << *i2 << endl;
            if (i1 == i2) continue;
            if (!getQueued(*i1, *i2) && Edge::intersect(&S->at(*i1), &S->at(*i2))) {
                Point p = intersection_point(*i1, *i2);
                if (p.y <= e.p.y) {
                    Q.emplace(p.x, p.y, *i1, *i2, INTERSECTION);
                    setQueued(*i1, *i2);
                }
            }
        }
    }
    Cache::setIntersect(e.e1, e.e2);
}

void Sweepline::sweep(vector<Edge> *set) {
    S = set;
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
        if (DEBUG) printStatus(e.p.y);
        sum++;
    }
    cout << "#events: " << sum << endl;
    removeBounds();
}