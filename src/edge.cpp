#include "edge.h"

// Edge representation and functionality
// Vincent Moonen

Edge::Edge(Vertex *v1, Vertex *v2) {
    this->v1 = v1;
    this->v2 = v2;
}

Edge::Edge(int x1, int y1, int x2, int y2) {
    this->v1 = new Vertex(x1, y1);
    this->v2 = new Vertex(x2, y2);
}

// Do these edges intersect?
// Vincent Moonen
bool Edge::intersect(Edge *e1, Edge *e2) {
    bool result = intersect_with_endpoint(e1, e2);
    if (!result) return false;
    // Check if edge are the same, if so they collide
    bool e1v1_e2v1 = Vertex::equal(e1->v1, e2->v1);
    bool e1v2_e2v1 = Vertex::equal(e1->v2, e2->v1);
    bool e1v1_e2v2 = Vertex::equal(e1->v1, e2->v2);
    bool e1v2_e2v2 = Vertex::equal(e1->v2, e2->v2);
    if ((e1v1_e2v1 && e1v2_e2v2) || (e1v1_e2v2 && e1v2_e2v1)) {
        return true;
    }
    // Check for individual endpoint equalities
    return (!(e1v1_e2v1 || e1v2_e2v1 || e1v1_e2v2 || e1v2_e2v2));
}

// Do these edges intersect, including endpoint?
// Vincent Moonen
bool Edge::intersect_with_endpoint(Edge *e1, Edge *e2) {
    Vertex *p1 = e1->v1; Vertex *q1 = e1->v2;
    Vertex *p2 = e2->v1; Vertex *q2 = e2->v2;
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
    if (o1 != o2 && o3 != o4) return true;
    if (o1 == 0 && on_edge(p1, p2, q1)) return true;
    if (o2 == 0 && on_edge(p1, q2, q1)) return true;
    if (o3 == 0 && on_edge(p2, p1, q2)) return true;
    if (o4 == 0 && on_edge(p2, q1, q2)) return true;
    return false;
}

// Helper for intersection, defines orientation of points
// Vincent Moonen
int Edge::orientation(Vertex *p, Vertex *q, Vertex *r) {
    long long val = (long long) (q->y - p->y) * (long long) (r->x - q->x) -
                (long long) (q->x - p->x) * (long long) (r->y - q->y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

// Is the point on the line segment pr
// Vincent Moonen
bool Edge::on_edge(Vertex *p, Vertex *q, Vertex *r) {
    return (q->x <= max(p->x, r->x) && q->x >= min(p->x, r->x) && 
            q->y <= max(p->y, r->y) && q->y >= min(p->y, r->y));
}