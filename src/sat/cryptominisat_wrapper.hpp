#ifndef CRYPTOMINISAT_WRAPPER_HPP
#define CRYPTOMINISAT_WRAPPER_HPP

#include "sat_solver_wrapper.hpp"
#include<cryptominisat5/cryptominisat.h>

#include<vector>

using namespace CMSat;

class CryptominisatWrapper : public SATSolverWrapper {
public:
    CryptominisatWrapper();
    int solve(std::vector<std::vector<int>> &sat, std::vector<int> &sol);
    ~CryptominisatWrapper();
private:
    SATSolver _solver;
};

#endif // CRYPTOMINISAT_WRAPPER_HPP