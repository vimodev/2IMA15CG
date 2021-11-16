#pragma once

#include "vertex.h"
#include <iostream>

using namespace std;

class Edge {
    public:
        Vertex *v1;
        Vertex *v2;
        Edge(Vertex *v1, Vertex *v2);
        Edge(int x1, int y1, int x2, int y2);
        static bool intersect(Edge *e1, Edge *e2);
        static bool intersect_with_endpoint(Edge *e1, Edge *e2);

    private:
        static int orientation(Vertex *p, Vertex *q, Vertex *r);
        static bool on_edge(Vertex *p, Vertex *q, Vertex *r);
};