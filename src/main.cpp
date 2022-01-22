#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <ratio>
#include <sys/wait.h>

#include "instance.h"
#include "cache.h"
#include "solution.h"
#include "solvers.h"

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

string benchSingle(string path) {
    string name = "../benchmark/sweepline.csv";
    std::ofstream benchmarkFile;
    benchmarkFile.open(name, fstream::app);
    Instance inst(path);
    auto result = Cache::benchmark(&inst);
    string res = inst.id + "," + to_string(std::get<0>(result).count()) + ", " + to_string(std::get<1>(result).count()) + "\n";
    benchmarkFile << res;
    return res;
}

void benchmarkSweepline() {
    pid_t c_pid;
    int runners = 4;
    for (const auto & entry : fs::directory_iterator("../instances")) {
        int status = 0;
        runners--;
        c_pid = fork();
        if (c_pid == 0) {
            string res = benchSingle(entry.path());
            exit(0);
        } else if(runners <= 0){
            wait(NULL);
            runners++;
        }
    }
}

int main(int argc, char **argv) {
    // We want to be able to cancel out of OMP parallel sections
    if(!omp_get_cancellation()) {
        putenv((char*) "OMP_CANCELLATION=true");
        execv(argv[0], argv);
    }

//    benchmarkSweepline();
//    benchSingle(argv[1]);
//    return 0;


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
            int bound = bound_sol->num_colors;
            for (int i = bound-1; i > 0; i--) {
                cout << "\n[INFO] Trying to find solution for " << i << " colors.\n" << endl;
                sol = EvolutionarySolver(i).solve(inst);
                if (!sol || !sol->check_validity()) break;
                cout << "\n[INFO] Found solution for " << i << " colors.\n" << endl;
                sol->to_file("../solutions/", true, "");
            }

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