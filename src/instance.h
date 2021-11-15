#pragma once

#include <json.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include "vertex.h"
#include "edge.h"

using json = nlohmann::json;
using namespace std;

const string INSTANCES_PATH = "../instances/";

class Instance {
    public:
        string type;
        string id;
        int n;
        int m;
        Vertex *vertices;
        Edge *edges;
        Instance(string filepath);
        ~Instance();
    private:
};