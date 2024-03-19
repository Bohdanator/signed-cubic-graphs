#ifndef CADICAL_WRAPPER_HPP
#define CADICAL_WRAPPER_HPP

#include "sat_solver_wrapper.hpp"
#include "../../cadical/src/cadical.hpp"

#include<vector>

class CadicalWrapper : public SATSolverWrapper {
public:
    CadicalWrapper();
    int solve(std::vector<std::vector<int>> &sat, std::vector<int> &sol);
    ~CadicalWrapper() override;
private:
    CaDiCaL::Solver * _solver;
};

#endif // CADICAL_WRAPPER_HPP