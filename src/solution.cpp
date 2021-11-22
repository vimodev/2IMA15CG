#include "solution.h"

Solution::Solution(Instance *instance) {
    this->instance = instance;
    this->num_colors = 0;
    this->colors = nullptr;
}

int Solution::initialize(int num_colors) {
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> distr(0, num_colors-1); // define the range

    // Initialize colors
    vector<int> *colors = new vector<int>;
    for (int i = 0; i < this->instance->m; i++) {
        colors->push_back(distr(gen));
    }

    this->num_colors = num_colors;
    this->colors = colors;

    return 0;
}

void Solution::to_file(const string& output_dir, bool include_num, const string& alg) {
    json j;
    j["type"] = "Solution_CGSHOP2022";
    j["instance"] = this->instance->id;
    j["num_colors"] = this->num_colors;
    json j_list(*this->colors);
    j["colors"] = j_list;
    string filepath;
    if (include_num) {
        filepath = output_dir + this->instance->id + "." + to_string(this->num_colors) + alg + ".json";
    } else {
        filepath = output_dir + this->instance->id + ".json";
    }
    std::ofstream output(filepath);
    output << std::setw(4) << j << std::endl;
}

// Check if a solution is valid
bool Solution::check_validity() {
    // Edges for easy access
    vector<Edge> *edges = this->instance->edges;
    bool result = true;

    // Multi thread this loop
    #pragma omp parallel for default(none) schedule(dynamic) shared(result, edges)
    for (int i = 1; i < this->instance->m; i++) {
        for (int j = 0; j < i; j++) {
            // If not the same color, continue
            if (this->colors->at(i) != this->colors->at(j)) continue;
            // Check for intersection
            if (Edge::intersect(&edges->at(i), &edges->at(j))) {
                // Write result atomically
                #pragma omp atomic write
                result = false;
                // Signal cancellation.
                #pragma omp cancel for
            }
            // Check if any thread issues cancellation
            #pragma omp cancellation point for
        }
    }
    // Return result
    return result;
}

// Get the number of clashes (wrong colorings) in a solution
int Solution::get_clashes() {
    // Edges for easy access
    vector<Edge> *edges = this->instance->edges;
    int clashes = 0;
    
    // Multi thread this loop
    #pragma omp parallel for default(none) schedule(dynamic) shared(clashes, edges)
    for (int i = 1; i < this->instance->m; i++) {
        for (int j = 0; j < i; j++) {
            // If not the same color, continue
            if (this->colors->at(i) != this->colors->at(j)) continue;
            // Check for intersection
            if (Edge::intersect(&edges->at(i), &edges->at(j))) {
                // Write result atomically
                #pragma omp atomic write
                clashes = clashes + 1;
            }
        }
    }
    // Return result
    return clashes;
}