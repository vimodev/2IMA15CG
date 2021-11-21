#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <omp.h>

#include "intersection_cache.h"
#include "instance.h"
#include "solution.h"
#include "solvers.h"

using namespace std;

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    // We want to be able to cancel out of OMP parallel sections
    if(!omp_get_cancellation()) {
        putenv((char*) "OMP_CANCELLATION=true");
        execv(argv[0], argv);
    }

    // Seed random
    srand (time(nullptr)); // NOLINT(cert-msc51-cpp)

    // for (const auto & entry : fs::directory_iterator("../instances")) {
    //     string p = entry.path();
    //     Instance inst(p);
    //     cout << "Solving " << inst.id << "..." << endl;
    //     Solution *sol = greedy(inst);
    //     if(sol->check_validity()) {
    //         cout << "Solution valid!" << endl;
    //     } else {
    //         cout << "Solution invalid!" << endl;
    //     }
    //     sol->to_file("../greedy_solutions/", true, "greedy");
    //     delete sol->colors;
    //     delete sol;
    //     delete inst.edges;
    //     delete inst.vertices;
    // }

    // Reading instance from json wokrs
    Instance inst("../instances/vispecn70501.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.edges->size() << " edges." << endl;
    IntersectionCache::set_instance(&inst);
    //AdjacencyList::initialize();

//    Solution *sol = degree_greedy(inst);
//    Solution *sol = IterativeGreedySolver(5).solve(inst);
    Solution *sol = GreedySolver().solve(inst);

    cout << "Solution found. Colors used: " << sol->num_colors << endl;
    cout << "Checking validity..." << endl;

    if(sol->check_validity()) {
        cout << "Solution valid!" << endl;
    } else {
        cout << "Solution invalid!" << endl;
    }

    sol->to_file("", false, "");

    return 0;
}