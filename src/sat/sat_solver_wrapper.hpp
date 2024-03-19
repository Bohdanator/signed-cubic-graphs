#ifndef SAT_SOLVER_WRAPPER_HPP
#define SAT_SOLVER_WRAPPER_HPP

#include<vector>

// Interface for a simple SAT solver wrapper class to provide single function solving one SAT instance
class SATSolverWrapper {
public:
    virtual ~SATSolverWrapper() = default;
    // Solve SAT instance contained in 'sat' vector. Return the result and the model in 'sol' vector.
    int solve(std::vector<std::vector<int>> &sat, std::vector<int> &sol);
};

#endif // SAT_SOLVER_WRAPPER_HPP