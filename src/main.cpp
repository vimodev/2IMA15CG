#include <stdio.h>
#include <iostream>
#include <string.h>
#include "instance.h"
#include "solution.h"

using namespace std;

int main(int argc, char **argv) {
    // Reading instance from json wokrs
    Instance inst("reecn73116.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.m << " edges." << endl;
    Edge e1(1, 1, 5, 2);
    Edge e2(3, 1, 4, 2);
    cout << Edge::intersect(&e1, &e2) << endl;
    // Edge intersection works
    long intersections = 0;
    #pragma omp parallel for schedule(dynamic) reduction(+:intersections)
    for (int i = 1; i < inst.m; i++) {
        for (int j = 0; j < i; j++) {
            intersections += (long) Edge::intersect(&inst.edges->at(i), &inst.edges->at(j));
        }
    }
    cout << "#intersections: " << intersections << endl;
    // Writing solution to json works
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