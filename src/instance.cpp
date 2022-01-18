#include "instance.h"

#define edge_limit 2107


std::ostream &operator<<(std::ostream &os, Vertex const &v) { 
            return os << "(" << v.x << "," << v.y << ")";
        }

// Create a problem instance from a parsed json
// Vincent Moonen
Instance::Instance(const string& filepath) {
    // Parse the file as a json
    // No robustness TODO
    json j;
    std::ifstream input_stream(filepath);
    input_stream >> j;
    // Copy the meta data
    this->type = j["type"];
    this->id = j["id"];
    this->n = j["n"];
    this->m = j["m"];
    this->m = min(this->m, edge_limit);
    // Populate vertices
    this->vertices = new vector<Vertex>;
    for (int i = 0; i < this->n; i++) {
        Vertex v(j["x"][i], j["y"][i]);
        this->vertices->push_back(v);
    }
    // Populate edges
    this->edges = new vector<Edge>;
    for (int i = 0; i < min((int) j["m"], edge_limit); i++) {
        int v1i = j["edge_i"][i];
        int v2i = j["edge_j"][i];
        Edge e(&this->vertices->at(v1i), &this->vertices->at(v2i));
        this->edges->push_back(e);
        cout << v1i << " " << v2i << " : " << this->vertices->at(v1i) << " - " << this->vertices->at(v2i) << endl;
    }
    assert((long unsigned int) this->n == this->vertices->size());
    assert((long unsigned int) this->m == this->edges->size());
}