#include "solution.h"

void Solution::toFile(string output_dir) {
    json j;
    j["type"] = this->type;
    j["instance"] = this->instance;
    j["num_colors"] = this->num_colors;
    json j_list(*this->colors);
    j["colors"] = j_list;
    string filepath = output_dir + this->instance + ".json";
    std::ofstream output(filepath);
    output << std::setw(4) << j << std::endl;
}