#include "instance.h"

// Create a problem instance from a parsed json
Instance::Instance(string filepath) {
    // Parse the file as a json
    // No robustness TODO
    json j;
    std::ifstream input_stream(INSTANCES_PATH + "reecn3382.instance.json");
    input_stream >> j;
    // Copy the meta data
    this->type = j["type"];
    this->id = j["id"];
    this->n = j["n"];
    this->m = j["m"];
    // Populate vertices
    this->vertices = (Vertex *) malloc(sizeof(Vertex) * this->n);
    for (int i = 0; i < this->n; i++) {
        Vertex v = {};
        v.x = j["x"][i];
        v.y = j["y"][i];
        this->vertices[i] = v;
    }
    // Populate edges
    this->edges = (Edge *) malloc(sizeof(Edge) * this->m);
    for (int i = 0; i < this->m; i++) {
        Edge e = {};
        int v1i = j["edge_i"][i];
        int v2i = j["edge_j"][i];
        e.v1 = &this->vertices[v1i];
        e.v2 = &this->vertices[v2i];
        this->edges[i] = e;
    }
}

Instance::~Instance() {
    free(this->vertices);
    free(this->edges);
}