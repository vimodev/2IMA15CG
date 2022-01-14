#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <omp.h>

#include "instance.h"
#include "cache.h"
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
    Instance inst(argv[1]);
    cout << "[INFO] " << inst.id << " has " << inst.vertices->size() << " vertices." << endl;
    cout << "[INFO] " << inst.id << " has " << inst.edges->size() << " edges." << endl;
    Cache::set_instance(&inst);
    Solution *sol;

    switch (*argv[2]) {
        case '3': { 
            cout << "[INFO] Running evolutionary solver." << endl;
            Solution *bound_sol = DSaturSolver().solve(inst);
            sol = EvolutionarySolver(bound_sol->num_colors-1).solve(inst);
            break;
        }
        case '2': {
            cout << "[INFO] Running DSatur solver." << endl;
            sol = DSaturSolver().solve(inst);
            break;
        }
        case '1': 
        default: {
            cout << "[INFO] Running greedy solver." << endl;
            sol = GreedySolver().solve(inst);
            break;
        }
    }

    cout << "[INFO] Solution found." << endl;

    cout << "\n  -- SOLUTION  SUMMARY --  " << endl;
    cout << "        Colors : " << sol->num_colors << endl;
    cout << "        Valid? : " << (sol->check_validity() ? "YES" : "NO") << endl;
    cout << "  - - - - - - - - - - - -  \n" << endl;

    cout << "[INFO] Writing solution to file." << endl;
    sol->to_file("../solutions/", true, "");

    cout << "\nDONE..." << endl;
    return 0;
}