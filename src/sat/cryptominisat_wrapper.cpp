#include "cryptominisat_wrapper.hpp"
#include<vector>

CryptominisatWrapper::CryptominisatWrapper() = default;
CryptominisatWrapper::~CryptominisatWrapper() = default;

int CryptominisatWrapper::solve(std::vector<std::vector<int>> &sat, std::vector<int> &sol) {
    std::vector<Lit> clause;
    int n = 0;
    for (auto f : sat) {
        for (auto x : f) {
            if (x > n) n = x;
        }
    }
    _solver.new_vars(n+1);
    for (auto f : sat) {
        clause.clear();
        for (auto x : f) {
            if (x > 0) {
                clause.push_back(Lit(x, true));
            } else {
                clause.push_back(Lit(-x, false));
            }
        }
        _solver.add_clause(clause);
    }
    lbool res = _solver.solve();
    if (res == l_False) {
        return 11;
    }
    sol.resize(n+1);
    for (int i = 1; i < n+1; i++) {
        sol[i] = (_solver.get_model()[i] == l_True ? 1 : 0);
    }
    return 10;
}
