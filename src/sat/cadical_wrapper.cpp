#include "cadical_wrapper.hpp"
#include<vector>
#include<iostream>

CadicalWrapper::CadicalWrapper() {
    _solver = new CaDiCaL::Solver();
}

int CadicalWrapper::solve(std::vector<std::vector<int>> &sat, std::vector<int> &sol) {
    int n = 0;
    for (auto f : sat) {
        for (auto x : f) {
            _solver->add(x);
            if (x > n) n = x;
        }
        _solver->add(0);
    }
    int result = _solver->solve();
    if (result != 10) {
        return result;
    }
    sol.resize(n+1);
    for (int i = 1; i < n+1; i++) {
        sol[i] = _solver->val(i);
    }
    return result;
}

CadicalWrapper::~CadicalWrapper() {
    delete _solver;
}
