#include <stdio.h>
#include <iostream>
#include <string.h>
#include "instance.h"
#include "solution.h"

using namespace std;

int main(int argc, char **argv) {
    Instance inst("reecn73116.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.m << " edges." << endl;
    Solution sol;
    sol.instance = inst.id;
    sol.type = inst.type;
    sol.num_colors = 5;
    sol.colors = new vector<int>;
    for (int i = 0; i < 5; i++) {
        sol.colors->push_back(i);
    }
    string dir = "";
    sol.toFile(dir);
    return 0;
}