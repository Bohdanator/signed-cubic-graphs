#include "kissat_wrapper.hpp"
#include<vector>
#include<iostream>

KissatWrapper::KissatWrapper() {
    _solver = kissat_init();
    kissat_set_option(_solver, "quiet", 1);
}

int KissatWrapper::solve(std::vector<std::vector<int>> &sat, std::vector<int> &sol) {
    int n = 0;
    for (auto f : sat) {
        for (auto x : f) {
            kissat_add(_solver, x);
            if (x > n) n = x;
        }
        kissat_add(_solver, 0);
    }
    int result = kissat_solve(_solver);
    if (result != 10) {
        return result;
    }
    sol.resize(n+1);
    for (int i = 1; i < n+1; i++) {
        sol[i] = kissat_value(_solver, i);
    }
    return result;
}

KissatWrapper::~KissatWrapper() {
    kissat_release(_solver);
}