#include "sweep.h"
#include "cache.h"

#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>
#include <limits>

#include "cache.h"

#define DEBUG false

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
        return min(e.v1->x, e.v2->x);
    } else {
        return e.v1->x + (e.v2->x - e.v1->x)*((y - e.v1->y)/(e.v2->y - e.v1->y));
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

static Point getLower(Edge e) {
    Vertex *v1 = e.v1;
    Vertex *v2 = e.v2;
    if (v1->y > v2->y) return {(long double)v2->x, (long double)v2->y};
    if (v1->y < v2->y) return {(long double)v1->x, (long double)v1->y};
    if (v1->x < v2->x) return {(long double)v2->x, (long double)v2->y};
    return {(long double)v1->x, (long double)v1->y};
}

long double get_slope(Edge e) {
    Point origin = getUpper(e);
    Point p = getLower(e);
    long double x = p.x - origin.x;
    long double y = p.y - origin.y;
    long double angle = atan2(y, x);
    if (angle < 0) angle += M_PI;
    return angle;
}

static vector<Edge> *S;
static vector<int> T;
// static priority_queue<Event> Q;
static set<Event> Q;
static set<Event> holding;
bitset<MAX_EDGES> queued[MAX_EDGES];
static bool doDebug = false;

string toString(EventType t) {
    if (t == UPPER) return "upper";
    if (t == LOWER) return "lower";
    return "intersect";
}

void printStatus(double long y) {
    cout << "STATUS at y:" << y << " -> ";
    for (int e : T) {
        cout << "[ edge" << e << " (" << get_x_on_e_with_y(S->at(e), y) << " | " << get_slope(S->at(e)) << " ) ]";
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

static StatusIter getIteratorRec(long double x, long double slope, long double y, int e, int l, int r) {
//    cout << "Searching for edge " << x << " , " << e << " at right " << r << " and left " << l <<endl;
    if (r >= l) {
        int mid = l + (r - l) / 2;
//        cout << "mid: " << mid << endl;
        if (T[mid] == e) {
//            cout << "T[mid]: " << T[mid] << endl;
//            cout << "Found edge at " << mid << endl;
            return T.begin() + mid;
        }
        Edge mid_e = S->at(T[mid]);
        long double mid_x = get_x_on_e_with_y(mid_e, y);
//        cout << "midx: " << mid_x << endl;
//        cout << "T[mid]: " << T[mid] << endl;
//        cout << (mid_x > x) << endl;
        long double margin = 0.00000001;
        if (abs(mid_x-x) < margin) {
//            cout << " close call " << endl;
            int leftIndex = mid;
            Edge leftEdge = S->at(T[leftIndex]);
            long double leftEdge_x = get_x_on_e_with_y(leftEdge, y);
            while(abs(leftEdge_x-x) < margin) {
                leftIndex--;
                leftEdge = S->at(T[leftIndex]);
                leftEdge_x = get_x_on_e_with_y(leftEdge, y);
            }

            int rightIndex = mid;
            Edge rightEdge = S->at(T[rightIndex]);
            long double rightEdge_x = get_x_on_e_with_y(rightEdge, y);
            while(abs(rightEdge_x-x) < margin) {
                rightIndex++;
                rightEdge = S->at(T[rightIndex]);
                rightEdge_x = get_x_on_e_with_y(rightEdge, y);
            }

            return find(T.begin() + leftIndex, T.begin()+rightIndex, e);
        }

        if (mid_x > x) {
            return getIteratorRec(x, slope, y, e, l, mid - 1);
        } else {
            return getIteratorRec(x, slope, y, e, mid + 1, r);
        }
    }
    return T.end();
}

static StatusIter getIterator(long double y, int e) {
//    for (auto edge: T) {
//        cout << edge << ", ";
//    }
//    cout << endl;
    Edge edge = S->at(e);
    long double x = get_x_on_e_with_y(edge, y);
    long double slope = get_slope(edge);
//    return find(T.begin(), T.end(), e);
    return getIteratorRec(x, slope, y, e, 0, T.size());
}

static StatusIter addStatusRec(long double x, long double slope, long double y, int e, int l, int r) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (mid >= T.size()) {
            T.insert(T.begin() + mid, e);
            return T.begin() + mid;
        }
        Edge mid_e = S->at(T[mid]);
        long double mid_x = get_x_on_e_with_y(mid_e, y);
        long double mid_slope = get_slope(mid_e);
        if (mid_x == x && mid_slope == slope) {
            T.insert(T.begin() + mid, e);
            return T.begin() + mid;
        }
        
        if (mid_x > x || (mid_x == x && slope < mid_slope)) {
            return addStatusRec(x, slope, y, e, l, mid - 1);
        } else {
            return addStatusRec(x, slope, y, e, mid + 1, r);
        }
    }
    T.insert(T.begin() + l, e);
    return T.begin() + l;
}

static StatusIter addStatus(long double y, int e) {
    Edge edge = S->at(e);
    long double x = get_x_on_e_with_y(edge, y);
    long double slope = get_slope(edge);
    if (T.size() == 0) {
        T.push_back(e);
        return T.begin();
    }
    return addStatusRec(x, slope, y, e, 0, T.size());
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
    addStatus(0, S->size()-2);
    addStatus(0, S->size()-1);
}

void removeBounds() {
    S->pop_back();
    S->pop_back();
}

pair<StatusIter, StatusIter> getEqualityRange(StatusIter iter, long double y) {
    long double x = get_x_on_e_with_y(S->at(*(iter)), y);

    StatusIter it1 = iter;
    long double start = x;
    int counter = 0;
    for (; it1 != T.begin(); --it1) {
        long double test = get_x_on_e_with_y(S->at(*it1), y);
        if (test != start) {
            start = test;
            counter++;
            if (counter > 1) break;
        }
    }

    StatusIter it2 = iter;
    start = x;
    counter = 0;
    for (; it2 != T.end(); ++it2) {
        long double test = get_x_on_e_with_y(S->at(*it2), y);
        if (test != start) {
            start = test;
            counter++;
            if (counter > 1) break;
        }
    }

    return {it1, it2};
}

void handleUpper(Event e) {
    if (DEBUG) cout << "[DEBUG] Upper endpoint of edge:" << e.e1 << " at (" << e.p.x << ", " << e.p.y << ")" << " with slope " << get_slope(S->at(e.e1))<< endl;
    auto loc = addStatus(e.p.y, e.e1);
    auto range = getEqualityRange(loc, e.p.y);
    range.second = loc + 1;
    for (auto other = range.first; other != range.second; other++) {
        if (loc == other) {
            continue;
        }
        if (!getQueued(*loc, *other) && Edge::intersect(&S->at(*loc), &S->at(*other))) {
            Point p = intersection_point(*loc, *other);
            Q.emplace(p.x, p.y, *loc, *other, INTERSECTION);
            setQueued(*loc, *other);
        }
    }
    range = getEqualityRange(loc, e.p.y);
    range.first = loc - 1;
    for (auto other = range.first; other != range.second; other++) {
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
    if (doDebug) cout << "[DEBUG] Lower endpoint of edge: " << e.e1 << " at (" << e.p.x << ", " << e.p.y << ")" << endl;
    auto loc = getIterator(e.p.y, e.e1);

    auto left = getEqualityRange(loc, e.p.y);
    left.second = loc + 1;

    auto right = getEqualityRange(loc, e.p.y);
    right.first = loc - 1;

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

pair<StatusIter, StatusIter> getRange(StatusIter i1, StatusIter i2, long double cur_y) {
    if (distance(i1, T.end()) > distance(i2, T.end())) return {i1, i2};
    return {i2, i1};
}

pair<StatusIter, StatusIter> swapSegments(int e1, int e2, long double cur_y) {
    auto i1 = getIterator(cur_y, e1);
    auto i2 = getIterator(cur_y, e2);
    iter_swap(i1, i2);
    return getRange(i1, i2, cur_y);
}

void handleIntersection(Event e) {
    if (Cache::intersects(e.e1, e.e2)) return;
    if (DEBUG) cout << "[DEBUG] Intersection between edge:" << e.e1 << " and edge:" << e.e2 << " at (" << e.p.x << ", " << e.p.y << ")" << endl;
    auto swapped = swapSegments(e.e1, e.e2, e.p.y);

    auto left = getEqualityRange(swapped.first, e.p.y);
    left.second = swapped.first + 1;

    auto right = getEqualityRange(swapped.second, e.p.y);
    right.first = swapped.second - 1;

    for (auto i1 = left.first; i1 != right.second; i1++) {
        for (auto i2 = left.first; i2 != right.second; i2++) {
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

void handleBulkIntersection(){
    if (DEBUG) cout << "[DEBUG] Bulk Intersection between edges: ";
    set<int> edgeNumbers;
    vector<int> edges;

    Event e = *holding.begin();

    for(auto el: holding) {
        edgeNumbers.insert(el.e1);
        edgeNumbers.insert(el.e2);
    }
    for (auto edge: edgeNumbers) {
        edges.push_back(edge);
    }

    sort(edges.begin(), edges.end(),  [](int a, int b){ return get_slope(S->at(a)) < get_slope(S->at(b)); });

    int min_index = -1;
    for (auto edge: edges) {
        auto it = getIterator(e.p.y, edge);
        if (it != T.end())
        {
            int index = it - T.begin();
            if (index < min_index || min_index == -1) {
                min_index = index;
            }
        }
    }
    int index = min_index;
    for (auto edge: edges) {
        T[index] = edge;
        index++;
    }

    for (auto edge1: edges) {
        for (auto edge2: edges) {
            if (edge1 != edge2) {
                Cache::setIntersect(edge1, edge2);
            }
        }
    }

    StatusIter leftEdge = getIterator(e.p.y, T[min_index]);
    auto left = getEqualityRange(leftEdge, e.p.y);
    left.second = leftEdge;
    left.first = leftEdge - 1;
    for (auto i1 = left.first; i1 != left.second; i1++) {
        if (!getQueued(*i1, *left.second) && Edge::intersect(&S->at(*i1), &S->at(*left.second))) {
            Point p = intersection_point(*i1, *left.second);
            if (p.y <= e.p.y) {
                Q.emplace(p.x, p.y, *i1, *left.second, INTERSECTION);
                setQueued(*i1, *left.second);
            }
        }
    }

    int max_index = min_index + edges.size() - 1;
    StatusIter rightEdge = getIterator(e.p.y, T[max_index]);
    auto right = getEqualityRange(rightEdge, e.p.y);
    right.second = rightEdge + 2;
    right.first = rightEdge;
    for (auto i1 = right.first; i1 != right.second; i1++) {
        if (!getQueued(*i1, *right.first) && Edge::intersect(&S->at(*i1), &S->at(*right.first))) {
            Point p = intersection_point(*i1, *right.first);
            if (p.y <= e.p.y) {
                Q.emplace(p.x, p.y, *i1, *right.first, INTERSECTION);
                setQueued(*i1, *right.first);
            }
        }
    }

    holding.clear();
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
        if (e.type == INTERSECTION) {
            holding.emplace(e.p.x, e.p.y, e.e1, e.e2, INTERSECTION);

            auto iter = Q.begin();
            while (iter->p.x == e.p.x && iter->p.y == e.p.y && iter->type == INTERSECTION) {
                holding.emplace(iter->p.x, iter->p.y, iter->e1, iter->e2, INTERSECTION);
                Q.erase(iter);
                iter = Q.begin();
            }

            if (holding.size() == 1) {
                holding.clear();
                handleIntersection(e);
            } else {
                handleBulkIntersection();
            }
        }

//        Edge left = S->at(S->size() - 2);
//        vector<int> old = T;
//
//        cout.precision(20);
//        long double prev_x = left.v1->x - 1;
//
//        bool cancer = false;
//        for (auto el : T) {
//            long double new_x = get_x_on_e_with_y(S->at(el), e.p.y);
//            if (prev_x - new_x > 0.00001) {
//                cout << "KANKER " << el << endl;
//                cancer = true;
//            }
//            prev_x = new_x;
//        }
//        if (cancer) printStatus(e.p.y);
//        if (cancer)  break;


        if (sum % 100000 == 0) {
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