#include "edge.h"

Edge::Edge(Vertex *v1, Vertex *v2) {
    this->v1 = v1;
    this->v2 = v2;
}

// Do the edges intersect?
bool Edge::intersect(Edge *e1, Edge *e2) {
    return ccw(e1->v1, e2->v1, e2->v2) != ccw(e1->v2, e2->v1, e2->v2) && 
                ccw(e1->v1, e1->v2, e2->v1) != ccw(e1->v1, e1->v2, e2->v2);
}

// Are the following 3 vertices in counter clockwise order?
// https://bryceboe.com/2006/10/23/line-segment-intersection-algorithm/
bool Edge::ccw(Vertex *a, Vertex *b, Vertex *c) {
    return (c->y - a->y) * (b->x - a->x) > (b->y - a->y) * (c->x - a->x);
}