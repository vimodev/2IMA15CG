#include "vertex.h"

Vertex::Vertex(int x, int y) {
    this->x = x; this->y = y;
}

bool Vertex::equal(Vertex *v1, Vertex *v2) {
    return (v1->x == v2->x && v1->y == v2->y);
}

std::ostream &operator<<(std::ostream &os, Vertex const &v) { 
    return os << "(" << v.x << "," << v.y << ")";
}