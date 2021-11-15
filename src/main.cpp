#include <stdio.h>
#include <iostream>
#include <string.h>
#include "instance.h"
#include "solution.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(int argc, char **argv) {

    srand (time(NULL));

    // Reading instance from json wokrs
    Instance inst("reecn73116.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.m << " edges." << endl;

    // Writing solution to json works
    Solution sol(&inst);
    sol.num_colors = 150;
    sol.colors = new vector<int>;
    for (int i = 0; i < inst.m; i++) {
        sol.colors->push_back(rand() % sol.num_colors);
    }
    sol.check_validity();
    return 0;
}