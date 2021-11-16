#include "instance.h"

// Create a problem instance from a parsed json
Instance::Instance(string filepath) {
    // Parse the file as a json
    // No robustness TODO
    json j;
    std::ifstream input_stream(INSTANCES_PATH + filepath);
    input_stream >> j;
    // Copy the meta data
    this->type = j["type"];
    this->id = j["id"];
    this->n = j["n"];
    this->m = j["m"];
    // Populate vertices
    this->vertices = new vector<Vertex>;
    for (int i = 0; i < this->n; i++) {
        Vertex v = {};
        v.x = j["x"][i];
        v.y = j["y"][i];
        this->vertices->push_back(v);
    }
    // Populate edges
    this->edges = new vector<Edge>;
    for (int i = 0; i < this->m; i++) {
        int v1i = j["edge_i"][i];
        int v2i = j["edge_j"][i];
        Edge e(&this->vertices->at(v1i), &this->vertices->at(v2i));
        this->edges->push_back(e);
    }
    assert((long unsigned int) this->n == this->vertices->size());
    assert((long unsigned int) this->m == this->edges->size());
}