#pragma once

#include "solution.h"

/**
 * Base class for solvers.
 */
class AbstractSolver {
public:

    AbstractSolver() = default;
    virtual string getName() = 0;
    virtual Solution *solve(Instance instance) = 0;
};

/**
 * Greedy solver that colors the first uncolored edge with a color
 * and then goes over all remaining uncolored edges to check for collisions with edges of the new color
 * and if there are none we give it the same color.
 */
class GreedySolver : public AbstractSolver {
public:
    GreedySolver(int N = numeric_limits<int>::max()) : AbstractSolver() {
        this->N = N;
    }

    string getName() override {
        return "GreedySolver";
    };

    int N;
    Solution *solve(Instance instance) override;
};

class IterativeGreedySolver : public AbstractSolver {
public:
    IterativeGreedySolver(int N = 1) : AbstractSolver() {
        this->N = N;
    }

    string getName() override {
        return "IterativeGreedySolver";
    };

    int N;
    Solution *solve(Instance instance) override;
};

class DegreeGreedySolver : public AbstractSolver {
public:
    string getName() override {
        return "DegreeGreedySolver";
    };

    Solution *solve(Instance instance) override;
};

class DSaturSolver: public AbstractSolver {
public:
    DSaturSolver(int N = numeric_limits<int>::max()) : AbstractSolver() {
        this->N = N;
    }

    string getName() override {
        return "DSaturSolver";
    };

    int N;
    Solution *solve(Instance instance) override;
};

class EvolutionarySolver : public AbstractSolver {
public:
    EvolutionarySolver(int N, int pop_size = 10, int iters = 50, int local_iters = 500) : AbstractSolver() {
        this->pop_size = pop_size;
        this->iters = iters;
        this->local_iters = local_iters;
        this->N = N;
    }

    string getName() override {
        return "EvolutionarySolver";
    };

    int pop_size;
    int iters;
    int local_iters;
    int N;
    Solution *solve(Instance instance) override;
};