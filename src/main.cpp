#include <stdio.h>
#include <iostream>
#include <json.h>
#include <fstream>
#include <string.h>

using json = nlohmann::json;
using namespace std;

string INSTANCES_PATH = "../instances/";

int main(int argc, char **argv) {
    json j;
    std::ifstream input_stream(INSTANCES_PATH + "reecn3382.instance.json");
    input_stream >> j;
    cout << j["id"] << endl;
    return 0;
}