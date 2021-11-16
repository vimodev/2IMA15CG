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
        /**
         * Function that tests whether a vertex is to the left or right of an edge.
         * @param p - Start Vertex
         * @param q - End Vertex
         * @param r - Vertex to test
         * @return 1 if r is to the right of pq, 2 if to the left.
         */
        static int orientation(Vertex *p, Vertex *q, Vertex *r);
        /**
         * Function that tests whether a vertex is on the given edge.
         * @param p - Start Vertex
         * @param q - End Vertex
         * @param r - Vertex to test
         * @return 1 if on edge, 0 otherwise.
         */
        static bool on_edge(Vertex *p, Vertex *q, Vertex *r);
};