#include<bits/stdc++.h>
#include "graph.h"
extern "C" {
    #include "kissat/src/kissat.h"
}

using namespace std;

void graph_to_SAT(Graph &graph, vector<vector<int>> &sat) {
    // colors: 3i + 3 = 0; 3i + 1 = +1; 3i + 2 = -1
    // half-edges (u,v); u < v -> 6i+1 = u_start; 6i+4 = v_start
    // edge constraints
    sat.clear();
    vector<int> signature;
    graph.signature(signature);
    for (int i = 0; i < graph.edges.size(); i++) {
        // two half-edges, each has at least one color
        sat.push_back({6*i+3, 6*i+1, 6*i+2});
        sat.push_back({6*i+6, 6*i+4, 6*i+5});
        // signature constraints for each half-edge
        if (signature[i] == 1) {
            sat.push_back({(-1)*(6*i+1), (-1)*(6*i+4)});
            sat.push_back({(-1)*(6*i+2), (-1)*(6*i+5)});
        } else {
            sat.push_back({(-1)*(6*i+1), (-1)*(6*i+5)});
            sat.push_back({(-1)*(6*i+2), (-1)*(6*i+4)});
        }
        // zero constraints for each half-edge
        sat.push_back({(-1)*(6*i+1), (-1)*(6*i+6)});
        sat.push_back({(-1)*(6*i+2), (-1)*(6*i+6)});
        sat.push_back({(-1)*(6*i+3), (-1)*(6*i+4)});
        sat.push_back({(-1)*(6*i+3), (-1)*(6*i+5)});
    }
    // different colors for each vertex constraints
    // each pair of half-edges at a vertex is different
    for (int i = 0; i < graph.n(); i++) {
        for (auto n1 : graph.vertex[i]) {
            for (auto n2 : graph.vertex[i]) {
                if (n1 <= n2) continue;
                // starting colors
                int e1 = 6*graph.adj_matrix[i][n1] + (n1 > i ? 4 : 1);
                int e2 = 6*graph.adj_matrix[i][n2] + (n2 > i ? 4 : 1);
                for (int j = 0; j < 3; j++) {
                    sat.push_back({-(e1+j), -(e2+j)});
                }
            }
        }
    }
}

int solve_sat_instance(vector<vector<int>> &sat, vector<int> &sol) {
    kissat* solver = kissat_init();
    kissat_set_option(solver, "quiet", 1);
    int n = 0;
    for (auto f : sat) {
        for (auto x : f) {
            kissat_add(solver, x);
            if (x > n) n = x;
        }
        kissat_add(solver, 0);
    }
    int result = kissat_solve(solver);
    if (result != 10) {
        return result;
    }
    sol.resize(n+1);
    for (int i = 1; i < n+1; i++) {
        sol[i] = kissat_value(solver, i);
    }
    return result;
}

void sat_result_to_coloring(Graph &graph, vector<int> &sol, vector<pair<int, int>> &coloring) {
    coloring.resize(graph.m(), make_pair(2, 2));
    for (int i = 0; i < graph.m(); i++) {
        if (sol[6*i+1] > 0) {
            coloring[i].first = 1;
        } else if (sol[6*i+2] > 0) {
            coloring[i].first = -1;
        } else if (sol[6*i+3] > 0) {
            coloring[i].first = 0;
        }
        if (sol[6*i+4] > 0) {
            coloring[i].second = 1;
        } else if (sol[6*i+5] > 0) {
            coloring[i].second = -1;
        } else if (sol[6*i+6] > 0) {
            coloring[i].second = 0;
        }
    }
}

int solve_graph(Graph &graph, vector<pair<int, int>> &coloring) {
    vector<vector<int>> sat_instance;
    graph_to_SAT(graph, sat_instance);

    vector<int> sol;
    int result = solve_sat_instance(sat_instance, sol);
    if (result != 10) {
        return result;
    }
    sat_result_to_coloring(graph, sol, coloring);
    return result;
}

void solve_each_signature(Graph &graph, bool print_unsat_only=false) {
    vector<bool> ST;
    spanning_tree(graph, ST);
    vector<pair<int, int>> coloring;
    int n_signatures = (1 << (graph.m() - graph.n() + 1));
    int result;

    for (int i = 0; i < n_signatures; i++) {
        result = solve_graph(graph, coloring);
        if (result != 10) {
            graph.print();
            cout << "UNSAT\n";
        } else if (!print_unsat_only) {
            graph.print();
            cout << "SAT\n";
            for (auto x : coloring) {
                cout << x.first << "," << x.second << " ";
            }
            cout << "\n";
        }
        next_signature(graph, ST);
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        string filename = argv[1];
        ifstream in(filename);
        while(!in.eof()) {
            Graph graph;
            graph.parse_from_stream(in);
            solve_each_signature(graph, true);
        }
    } else {
        while(!cin.eof()) {
            Graph graph;
            graph.parse_from_stream(cin);
            solve_each_signature(graph, true);
        }
    }
}
