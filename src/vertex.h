#pragma once

#include <iostream>

using namespace std;

class Vertex {
    public:
        Vertex(int x, int y);
        static bool equal(Vertex *v1, Vertex *v2);
        int x;
        int y;
};