#include "vertex.h"

// Vertex representation
// Vincent Moonen

Vertex::Vertex(int x, int y) {
    this->x = x; this->y = y;
}

bool Vertex::equal(Vertex *v1, Vertex *v2) {
    return (v1->x == v2->x && v1->y == v2->y);
}