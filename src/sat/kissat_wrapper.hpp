#ifndef KISSAT_WRAPPER_HPP
#define KISSAT_WRAPPER_HPP

#include "sat_solver_wrapper.hpp"

extern "C" {
    #include "../../kissat/src/kissat.h"
}

#include<vector>

class KissatWrapper : public SATSolverWrapper {
public:
    KissatWrapper();
    int solve(std::vector<std::vector<int>> &sat, std::vector<int> &sol);
    ~KissatWrapper() override;
private:
    kissat * _solver;
};

#endif // KISSAT_WRAPPER_HPP