#include "sweep.h"

#include <queue>
#include <map>
#include <algorithm>

#include "cache.h"

Point::Point(long double x, long double y) {
    this->x = x; this->y = y;
}

Point::Point() {
    this->x = 0.0; this->y = 0.0;
}

Event::Event(long double x, long double y, Edge *e1, Edge *e2, const string& type) {
    this->p.x = x; this->p.y = y;
    this->e1 = e1; this->e2 = e2;
    this->type = type;
}

void insert_endpoints(priority_queue<Event> *Q, vector<Edge> *S) {
    for (int i = 0; i < S->size(); i++) {
        Vertex *v1 = S->at(i).v1;
        Vertex *v2 = S->at(i).v2;
        // v1 is upper endpoint
        if (v1->y > v2->y) {
            Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, &S->at(i), (Edge *) nullptr, "upper");
            Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, &S->at(i), (Edge *) nullptr, "lower");
        // v2 is upper endpoint
        } else if (v1->y < v2->y) {
            Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, &S->at(i), (Edge *) nullptr, "upper");
            Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, &S->at(i), (Edge *) nullptr, "lower");
        // Left endpoint is upper on equality
        } else if (v1->y == v2->y) {
            if (v1->x < v2->x) {
                Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, &S->at(i), (Edge *) nullptr, "upper");
                Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, &S->at(i), (Edge *) nullptr, "lower");
            } else {
                Q->emplace((long double) 1.0 * v2->x, (long double) 1.0 * v2->y, &S->at(i), (Edge *) nullptr, "upper");
                Q->emplace((long double) 1.0 * v1->x, (long double) 1.0 * v1->y, &S->at(i), (Edge *) nullptr, "lower");
            }
        }
    }
}

Point intersection_point(Edge e1, Edge e2) {
    long double x1 = e1.v1->x; long double y1 = e1.v1->y;
    long double x2 = e1.v2->x; long double y2 = e1.v2->y;
    long double x3 = e2.v1->x; long double y3 = e2.v1->y;
    long double x4 = e2.v2->x; long double y4 = e2.v2->y;
    long double D = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    long double x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / D;
    long double y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / D;
    return {x, y};
}

void handle_event(Event e, priority_queue<Event> *Q, multimap<long double, Edge*> *T) {
    // Handle upper endpoint event
    if (e.type == "upper") {
        // Insert edge into status
        T->insert({e.p.x, e.e1});
        multimap<long double, Edge *>::iterator iter = T->find(e.p.x);
        // Check intersection to the left
        if (iter != T->begin() && Edge::intersect(iter->second, prev(iter)->second)) {
            Point p = intersection_point(*iter->second, *prev(iter)->second);
            Q->emplace(p.x, p.y, prev(iter)->second, iter->second, "intersect");
        }
        // And to the right
        if (iter != prev(T->end()) && Edge::intersect(iter->second, next(iter)->second)) {
            Point p = intersection_point(*iter->second, *next(iter)->second);
            Q->emplace(p.x, p.y, iter->second, next(iter)->second, "intersect");
        }
    // Handle lower endpoint event
    } else if (e.type == "lower") {
        multimap<long double, Edge *>::iterator iter = T->find(e.p.x);
        // If left and right intersect below the line, we add that to event
        if (iter != T->begin() && iter != prev(T->end()) && Edge::intersect(prev(iter)->second, next(iter)->second)) {
            Point p = intersection_point(*prev(iter)->second, *next(iter)->second);
            if (p.y < e.p.y) {
                Q->emplace(p.x, p.y, prev(iter)->second, next(iter)->second, "intersect");
            }
        }
        // Erase the line segment
        for (auto i : *T) {
            if (i.second == e.e1) {
                T->erase(i.first);
                break;
            }
        }
    } else if (e.type == "intersect") {
        auto left = T->begin(); auto right = T->begin();
        for (auto i = T->begin(); i != T->end(); i++) {
            if (i->second == e.e1) {
                left = i;
            }
            if (i->second == e.e2) {
                right = i;
            }
        }
        auto left_edge = left->second; auto right_edge = right->second;
        auto left_x = left->first; auto right_x = right->first;
        T->erase(left_x); T->erase(right_x);
        T->insert({left_x, right_edge}); T->insert({right_x, left_edge});
        multimap<long double, Edge *>::iterator iter = T->find(left_x);
        // If left has intersection to the left now
        if (iter != T->begin() && Edge::intersect(prev(iter)->second, iter->second)) {
            Point p = intersection_point(*prev(iter)->second, *iter->second);
            if (p.y < e.p.y) {
                Q->emplace(p.x, p.y, prev(iter)->second, iter->second, "intersect");
            }
        }
        iter = T->find(right_x);
        // If right has intersection to the right now
        if (iter != prev(T->end()) && Edge::intersect(iter->second, next(iter)->second)) {
            Point p = intersection_point(*iter->second, *next(iter)->second);
            if (p.y < e.p.y) {
                Q->emplace(p.x, p.y, iter->second, next(iter)->second, "intersect");
            }
        }
        Cache::cache[i][j] = true;
    }
}

void sweepline(vector<Edge> *S) {
    priority_queue<Event> Q;
    multimap<long double, Edge*> T;
    insert_endpoints(&Q, S);
    while (!Q.empty()) {
        Event e = Q.top();
        Q.pop();
        handle_event(e, &Q, &T);
    }
}