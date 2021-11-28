#pragma once

#include <json.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include "vertex.h"
#include "edge.h"
#include <cassert>

using json = nlohmann::json;
using namespace std;

#define MAX_EDGES 50000
#define MAX_COLORS 100

const string INSTANCES_PATH = "../instances/";

class Instance {
    public:
        string type; // Should be Instance_CGSHOP2022
        string id; // ID of the instance
        int n; // Amount of vertices
        int m; // Amount of edges
        vector<Vertex> *vertices;
        vector<Edge> *edges;
        explicit Instance(const string& filepath);
    private:
};