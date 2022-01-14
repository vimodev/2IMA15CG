#include "../solvers.h"
#include "../searchers.h"
#include "../cache.h"

// Evolutionary strategy making use of DSatur and TabuSearch
// Maxim Snoep
Solution* EvolutionarySolver::solve(Instance instance) {
    random_device rd; // obtain a random number from hardware
    mt19937 gen(rd()); // seed the generator
    uniform_int_distribution<> distr(0, this->pop_size-1); // define the range

    auto* population = new vector<Solution*>;

    for (int i = 0; i < this->pop_size; i++) {
        Solution *sol = DSaturSolver(this->N).solve(instance);
        population->push_back(sol);
        std::cout << "[INFO] Solution #" << i << " added to population with " << sol->get_clashes() << " clashes." << std::endl;
    }

    for (int i = 0; i < this->iters; i++) {
        std::cout << "\n[INFO] Iteration " << (i+1) << "/" << this->iters << " of HEA." << std::endl;

        int parent1_i = distr(gen);
        Solution *parent1 = population->at(parent1_i);
        int parent2_i = distr(gen);
        Solution *parent2 = population->at(parent2_i);
        std::cout << "[INFO] Solution #" << parent1_i << " (clashes=" << parent1->get_clashes() << ") and solution #" << parent2_i << " (clashes=" << parent2->get_clashes() << ") randomly chosen to do the rumpy-pumpy." << std::endl; 

        Solution *offspring = new Solution(&instance);
        offspring->initialize(this->N);

        vector< set<int> >* parent1_c = new vector< set<int> >;
        for (int c = 0; c < parent1->num_colors; c++) {
            parent1_c->push_back({});
        }
        for (int k = 0; k < instance.m; k++) {
            parent1_c->at(parent1->colors->at(k)).insert(k);
        }
        
        vector< set<int> >* parent2_c = new vector< set<int> >;
        for (int c = 0; c < parent2->num_colors; c++) {
            parent2_c->push_back({});
        }
        for (int k = 0; k < instance.m; k++) {
            parent2_c->at(parent2->colors->at(k)).insert(k);
        }

        int largest_color_class_size = -1;
        int largest_color_class = -1;
        int is_parent1 = -1;
        for (int c = 0; c < parent1->num_colors; c++) {
            int size_color_class = parent1_c->at(c).size();
            if (size_color_class > largest_color_class_size) {
                largest_color_class_size = size_color_class;
                largest_color_class = c;
                is_parent1 = 1;
            }
        }

        for (int c = 0; c < parent2->num_colors; c++) {
            int size_color_class = parent2_c->at(c).size();
            if (size_color_class > largest_color_class_size) {
                largest_color_class_size = size_color_class;
                largest_color_class = c;
                is_parent1 = 0;
            }
        }

        vector< set<int> >* largest;
        vector< set<int> >* other;
        vector< int > other_;
        if (is_parent1) { 
            largest = parent1_c;
            other = parent2_c;
            other_ = *parent2->colors;
        } else {
            largest = parent2_c;
            other = parent1_c;
            other_ = *parent1->colors;
        }

        for (int v : largest->at(largest_color_class)) {
            auto it = other->at(other_.at(v)).find(v);
            other->at(other_.at(v)).erase(it);
            offspring->colors->at(v) = largest_color_class;
        }
        largest->at(largest_color_class) = {};


        for (int colors_used = 1; colors_used <= this->N; colors_used++) {
            largest_color_class_size = -1;
            largest_color_class = -1;
            if (is_parent1) { 
                largest = parent1_c;
                other = parent2_c;
                other_ = *parent2->colors;
                is_parent1 = !is_parent1;
            } else {
                largest = parent2_c;
                other = parent1_c;
                other_ = *parent1->colors;
                is_parent1 = !is_parent1;
            }

            for (int c = 0; c < parent1->num_colors; c++) {
                int size_color_class = largest->at(c).size();
                if (size_color_class > largest_color_class_size) {
                    largest_color_class_size = size_color_class;
                    largest_color_class = c;
                }
            }

            if (!largest_color_class_size) {
                break;
            }

            for (int v : largest->at(largest_color_class)) {
                auto it = other->at(other_.at(v)).find(v);
                other->at(other_.at(v)).erase(it);
                offspring->colors->at(v) = largest_color_class;
            }
            largest->at(largest_color_class) = {};
        }

        cout << "[INFO] Offspring has " << offspring->get_clashes() << " clashes." << endl;

        TabuSearcher().search(offspring, local_iters);
        if (offspring->get_clashes() == 0) {
            return offspring;
        }

        if (parent1->get_clashes() > parent2->get_clashes()) {
            population->at(parent1_i) = offspring;
            
        } else {
            population->at(parent2_i) = offspring;
        }
    }

    std::cout << "\n[INFO] HEA could not find a solution with 0 clashes." << std::endl;
    return nullptr;
}
