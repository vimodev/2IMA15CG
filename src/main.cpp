#include <stdio.h>
#include <iostream>
#include <string.h>
#include "instance.h"
#include "solution.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

using namespace std;

int main(int argc, char **argv) {
    // We want to be able to cancel out of OMP parallel sections
    if(!omp_get_cancellation()) {
      putenv("OMP_CANCELLATION=true");
      execv(argv[0], argv);
    }

    // Seed random
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
    cout << sol.check_validity() << endl;
    return 0;
}