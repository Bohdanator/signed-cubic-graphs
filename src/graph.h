#include<vector>
#include<string>
#include<iostream>
#include<queue>
#include "cadical/src/cadical.hpp"
#include<cryptominisat5/cryptominisat.h>
extern "C" {
    #include "kissat/src/kissat.h"
}

using namespace std;
using namespace CMSat;

struct Graph {
    vector<vector<int>> vertex; // <neighbor, edge>
    vector<vector<int>> edges; // [u, v, sign]
    vector<vector<int>> adj_matrix;

    Graph() = default;

    Graph(int n) {
        vertex = vector<vector<int>>(n, vector<int>(0));
        adj_matrix = vector<vector<int>>(n, vector<int>(n, -1));
        edges = vector<vector<int>>(0);
    }

    int n() { return vertex.size(); }

    int m() { return edges.size(); }

    void clear() {
        vertex.clear();
        edges.clear();
        adj_matrix.clear();
    }

    void print(ostream &out = cout) {
        out << vertex.size() << " " << edges.size() << "\n";
        for (int i = 0; i < edges.size(); i++) {
            for (int j = 0; j < edges[i].size(); j++) {
                out << edges[i][j] << ",";
            }
            out << " ";
        }
        out << "\n";
    }

    void add_edge(int u, int v, int sign=0) {
        if (adj_matrix[u][v] > -1) return;
        vector<int> e = {u, v, sign};
        vertex[e[0]].push_back(e[1]);
        vertex[e[1]].push_back(e[0]);
        adj_matrix[e[0]][e[1]] = edges.size();
        adj_matrix[e[1]][e[0]] = edges.size();
        edges.push_back(e);
    }

    void parse(int n, vector<vector<int>> &new_edges) {
        vertex.resize(n, vector<int>(0));
        adj_matrix.resize(n, vector<int>(n, -1));
        for (int i = 0; i < new_edges.size(); i++) {
            auto e = new_edges[i];
            add_edge(e[0], e[1], (e.size() >= 3 ? e[2] : 0));
        }
    }

    int parse_from_stream(istream &stream) {
        string buf;
        int n, m;
        string u, v, s;

        if (stream >> n >> m) {
            vertex.resize(n, vector<int>(0));
            adj_matrix.resize(n, vector<int>(n, -1));
            for (int i = 0; i < m; i++) {
                stream >> buf;
                stringstream ss(buf);
                getline(ss, u, ',');
                getline(ss, v, ',');
                getline(ss, s, ',');
                add_edge(stoi(u), stoi(v), stoi(s));
            }
            return 1;
        }
        return 0;
    }

    void graph_to_dot_colors(ostream &stream, vector<pair<int, int>> colors) {
        stream << "graph unsat_" << n() << "_" << m() << " {\n";
        stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
        for (int i = 0; i < n(); i++)
            stream << "\t" << i << " [shape=circle];\n";
        for (int i = 0; i < m(); i++)
            stream << "\t" << edges[i][0] << " -- " << edges[i][1] << (edges[i][2] > 0 ? " [style=dashed," : "[") << "headlabel=" << colors[i].first << ", taillabel=" << colors[i].second << "]" << ";\n";
        stream << "}";
    }

    void graph_to_dot(ostream &stream) {
        stream << "graph unsat_" << n() << "_" << m() << " {\n";
        stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
        for (int i = 0; i < n(); i++)
            stream << "\t" << i << " [shape=circle];\n";
        for (int i = 0; i < m(); i++)
            stream << "\t" << edges[i][0] << " -- " << edges[i][1] << (edges[i][2] > 0 ? " [style=dashed," : "[") << "]" << ";\n";
        stream << "}";
    }

    void graph_to_dot_ST(ostream &stream, vector<bool> &ST) {
        stream << "graph unsat_" << n() << "_" << m() << " {\n";
        stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
        for (int i = 0; i < n(); i++)
            stream << "\t" << i << " [shape=circle];\n";
        for (int i = 0; i < m(); i++)
            stream << "\t" << edges[i][0] << " -- " << edges[i][1] << (ST[i] ? "[style=bold, color=red" : (edges[i][2] > 0 ? " [style=dashed," : "[") )<< "]" << ";\n";
        stream << "}";
    }

    void signature(vector<int> &dest) {
        dest.resize(edges.size());
        for (int i = 0; i < edges.size(); i++) {
            dest[i] = edges[i][2];
        }
    }

    void adj(vector<vector<bool>> &matrix) {
        matrix.resize(n(), vector<bool>(n(), false));
    }
};


int number_of_triangles(Graph& graph) {
    int t = 0;
    for (int initial = 0; initial < graph.n(); initial++) {
        for (auto neigh : graph.vertex[initial]) {
            for (int common = 0; common < graph.n(); common++) {
                if ((common == initial) || (common == neigh)) continue;
                t += ((graph.adj_matrix[initial][common] >= 0) && (graph.adj_matrix[neigh][common] >= 0));
            }
        }
    }
    return t / 6;
}

void spanning_tree(Graph& graph, vector<bool>& ST) {
    ST.resize(graph.m(), false);
    queue<int> Q;
    vector<bool> seen(graph.m(), 0);
    Q.push(0);
    int v;
    while(!Q.empty()) {
        v = Q.front();
        Q.pop();
        seen[v] = true;

        for (auto neighbor : graph.vertex[v]) {
            if (!seen[neighbor]) {
                ST[graph.adj_matrix[v][neighbor]] = true;
                Q.push(neighbor);
                seen[neighbor] = true;
            }
        }
    }
}

void next_signature(Graph &graph, vector<bool>& ST) {
    int i = 0;
    while ((i < graph.m()) && (graph.edges[i][2] == 1 || ST[i])) {
        graph.edges[i][2] = 0;
        i++;
    }
    if (i < graph.edges.size()) graph.edges[i][2] = 1;
}

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

int solve_sat_instance_cadical(vector<vector<int>> &sat, vector<int> &sol) {
    CaDiCaL::Solver * solver = new CaDiCaL::Solver();
    int n = 0;
    for (auto f : sat) {
        for (auto x : f) {
            solver->add(x);
            if (x > n) n = x;
        }
        solver->add(0);
    }
    int result = solver->solve();
    if (result != 10) {
        return result;
    }
    sol.resize(n+1);
    for (int i = 1; i < n+1; i++) {
        sol[i] = solver->val(i);
    }
    delete solver;
    return result;
}

int solve_sat_instance_kissat(vector<vector<int>> &sat, vector<int> &sol) {
    kissat * solver = kissat_init();
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
    kissat_release(solver);
    return result;
}

int solve_sat_instance_cryptominisat(vector<vector<int>> &sat, vector<int> &sol) {
    SATSolver solver;
    vector<Lit> clause;
    int n = 0;
    for (auto f : sat) {
        for (auto x : f) {
            if (x > n) n = x;
        }
    }
    solver.new_vars(n+1);
    for (auto f : sat) {
        clause.clear();
        for (auto x : f) {
            if (x > 0) {
                clause.push_back(Lit(x, true));
            } else {
                clause.push_back(Lit(-x, false));
            }
        }
        solver.add_clause(clause);
    }
    lbool res = solver.solve();
    if (res == l_False) {
        return 11;
    }
    sol.resize(n+1);
    for (int i = 1; i < n+1; i++) {
        sol[i] = (solver.get_model()[i] == l_True ? 1 : 0);
    }
    return 10;
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
    int result = solve_sat_instance_cryptominisat(sat_instance, sol);
    if (result != 10) {
        return result;
    }
    sat_result_to_coloring(graph, sol, coloring);
    return result;
}
