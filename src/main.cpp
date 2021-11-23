#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <omp.h>

#include "cache.h"
#include "instance.h"
#include "solution.h"
#include "solvers.h"
#include "searchers.h"

using namespace std;

namespace fs = std::filesystem;

void benchmark(AbstractSolver* solver){
    string name = "../benchmark/benchmark-" + solver->getName() + ".csv";
    std::ofstream benchmarkFile(name);
    for (const auto & entry : fs::directory_iterator("../instances")) {
        benchmarkFile.open(name, fstream::app);
        string p = entry.path();
        Instance inst(p);
        cout << "Solving " << inst.id << "..." << endl;
        Cache::set_instance(&inst);
        Solution *sol = solver->solve(inst);

        if(sol->check_validity()) {
            cout << "Solution valid!" << endl;
        } else {
            cout << "Solution invalid!" << endl;
        }

        sol->to_file("../benchmark/solutions/", true, solver->getName());
        benchmarkFile << inst.id << "," << sol->num_colors << "\n";
        benchmarkFile.close();
        delete sol->colors;
        delete sol;
        delete inst.edges;
        delete inst.vertices;
    }
}

int main(int argc, char **argv) {
    // We want to be able to cancel out of OMP parallel sections
    if(!omp_get_cancellation()) {
        putenv((char*) "OMP_CANCELLATION=true");
        execv(argv[0], argv);
    }

    // Seed random
    srand (time(nullptr)); // NOLINT(cert-msc51-cpp)

    // Reading instance from json wokrs
    Instance inst("../instances/reecn3382.instance.json");
    cout << inst.id << " has " << inst.vertices->size() << " vertices and " << inst.edges->size() << " edges." << endl;
    Cache::set_instance(&inst);

//    Solution *sol = degree_greedy(inst);
//    Solution *sol = IterativeGreedySolver(10).solve(inst);
    Solution *sol = new Solution(&inst);
    sol->initialize(150);
    TabuSearcher().search(sol, 100000);

    cout << "Solution found. Colors used: " << sol->num_colors << endl;
    cout << "Checking validity..." << endl;

    if(sol->check_validity()) {
        cout << "Solution valid!" << endl;
    } else {
        cout << "Solution invalid!" << endl;
    }

    sol->to_file("../solutions/", true, "");

    return 0;
}