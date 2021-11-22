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