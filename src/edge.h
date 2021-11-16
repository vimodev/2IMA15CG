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

    private:
        static bool ccw(Vertex *a, Vertex *b, Vertex *c);
};