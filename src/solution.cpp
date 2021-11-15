#include "solution.h"

Solution::Solution(Instance *instance) {
    this->instance = instance;
}

void Solution::to_file(string output_dir) {
    json j;
    j["type"] = this->instance->type;
    j["instance"] = this->instance->id;
    j["num_colors"] = this->num_colors;
    json j_list(*this->colors);
    j["colors"] = j_list;
    string filepath = output_dir + this->instance->id + ".json";
    std::ofstream output(filepath);
    output << std::setw(4) << j << std::endl;
}

bool Solution::check_validity() {
    vector<Edge> *edges = this->instance->edges;
    long intersections = 0;
    #pragma omp parallel for schedule(dynamic) reduction(+:intersections)
    for (int i = 1; i < this->instance->m; i++) {
        for (int j = 0; j < i; j++) {
            if (this->colors->at(i) != this->colors->at(j)) continue;
            intersections += (long) Edge::intersect(&edges->at(i), &edges->at(j));
        }
    }
    cout << intersections << endl;
    return true;
}