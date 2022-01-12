#pragma once

#include "solution.h"

/**
 * Base class for solvers.
 */
class AbstractSearcher {
public:

    AbstractSearcher() = default;
    virtual string getName() = 0;
    virtual Solution *search(Solution* sol, int iterations) = 0;
};

class TabuSearcher: public AbstractSearcher {
    public:
        string getName() override {
            return "TabuSearcher";
        };

        Solution *search(Solution* sol, int iterations) override;
    private:
        static int T[MAX_EDGES][MAX_COLORS];
        static int C[MAX_EDGES][MAX_COLORS];
};

class IterativeSearcher: public AbstractSearcher {
    public:
        string getName() override {
            return "IterativeSearcher";
        };

        Solution *search(Solution* sol, int iterations) override;
};