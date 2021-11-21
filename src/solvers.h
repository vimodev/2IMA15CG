#pragma once

#include "solution.h"

/**
 * Base class for solvers.
 */
class AbstractSolver {
    public:
    AbstractSolver() = default;
        virtual Solution *solve(Instance instance) = 0;
};

/**
 * Greedy solver that colors the first uncolored edge with a color
 * and then goes over all remaining uncolored edges to check for collisions with edges of the new color
 * and if there are none we give it the same color.
 */
class GreedySolver: AbstractSolver{
    public:
    Solution *solve(Instance instance) override;
};

class IterativeGreedySolver: AbstractSolver{
public:
    IterativeGreedySolver(int N): AbstractSolver() {
        this->N = N;
    }
    int N = 0;
    Solution *solve(Instance instance) override;
};

class DegreeGreedySolver: AbstractSolver{
public:
    Solution *solve(Instance instance) override;
};