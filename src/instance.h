#pragma once

#include <json.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "vertex.h"
#include "edge.h"

using json = nlohmann::json;
using namespace std;

const string INSTANCES_PATH = "../instances/";

class Instance {
    public:
        string type; // Should be Instance_CGSHOP2022
        string id; // ID of the instance
        int n; // Amount of vertices
        int m; // Amount of edges
        vector<Vertex> *vertices;
        vector<Edge> *edges;
        Instance(string filepath);
        ~Instance();
    private:
};