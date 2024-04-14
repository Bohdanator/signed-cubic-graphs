#include "graph.hpp"

#include<vector>
#include<string>
#include<sstream>
#include<iostream>
#include<queue>

int parse_graph(Graph &graph, istream &stream) {
    string buf, u, v, s;
    int n, m;
    graph.clear();

    if (stream >> n >> m) {
        graph.init(n);
        for (int i = 0; i < m; i++) {
            stream >> buf;
            std::stringstream ss(buf);
            getline(ss, u, ',');
            getline(ss, v, ',');
            getline(ss, s, ',');
            graph.add_edge(stoi(u), stoi(v), stoi(s));
        }
        return 1;
    }
    return 0;
}

void print_graph(Graph &graph, ostream &out) {
    out << graph.vertex.size() << " " << graph.edges.size() << "\n";
    for (uint i = 0; i < graph.edges.size(); i++) {
        for (uint j = 0; j < graph.edges[i].size(); j++) {
            out << graph.edges[i][j] << ",";
        }
        out << " ";
    }
    out << "\n";
}

void graph_to_signed_adj_matrix(Graph &graph, vector<vector<int>> &matrix) {
    matrix.resize(graph.n(), vector<int>(graph.n(), 0));
    for (auto e : graph.edges) {
        if (e[2]) {
            matrix[e[1]][e[0]] = 1;
            continue;
        }
        matrix[e[0]][e[1]] = 1;
    }
}

void print_graph_signed_adj(Graph &graph, ostream &out) {
    vector<vector<int>> matrix;
    graph_to_signed_adj_matrix(graph, matrix);
    cout << "n=" << graph.n() << "\n";
    for (auto &x : matrix) {
        for (auto y : x) {
            out << y;
        }
        out << "\n";
    }
}

void print_graph_adj(Graph &graph, ostream &out) {
    cout << "n=" << graph.n() << "\n";
    for (auto &row : graph.adj_matrix) {
        for (auto cell : row) {
            if (cell == -1) {
                out << 0;
                continue;
            }
            out << 1;
        }
        out << "\n";
    }
}

void print_graph_edge_list(Graph &graph, ostream &out) {
    out << graph.n() << " " << graph.m() << "\n";
    for (auto edge : graph.edges) {
        cout << edge[0] << " " << edge[1] << "  ";
    }
    cout << "\n";
}

void graph_from_signed_adj_matrix_stream(Graph &graph, istream &in) {

}

int graph_from_edge_list(Graph &graph, istream &stream) {
    int n, m;
    if (stream >> n >> m) {
        graph.clear();
        graph.init(n);
        int a, b;
        for (auto i = 0; i < m; i++) {
            stream >> a >> b;
            graph.add_edge(a,b);
        }
        return 1;
    }
    return 0;
}

void graph_from_signed_adj_matrix(Graph &graph, vector<vector<int>> &matrix) {
    graph.clear();
    graph.init(matrix.size());
    for (uint i = 0; i < matrix.size(); i++) {
        for (uint j = i+1; j < matrix.size(); j++) {
            if (matrix[i][j]) {
                graph.add_edge(i, j, 0);
            } else if (matrix[j][i]) {
                graph.add_edge(i, j, 1);
            }
        }
    }
}

void graph_to_dot_colors(Graph &graph, ostream &stream, vector<pair<int, int>> colors) {
    stream << "graph unsat_" << graph.n() << "_" << graph.m() << " {\n";
    stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
    for (int i = 0; i < graph.n(); i++)
        stream << "\t" << i << " [shape=circle];\n";
    for (int i = 0; i < graph.m(); i++)
        stream << "\t" << graph.edges[i][0] << " -- " << graph.edges[i][1] << (graph.edges[i][2] > 0 ? " [style=dashed," : "[") << "headlabel=" << colors[i].first << ", taillabel=" << colors[i].second << "]" << ";\n";
    stream << "}";
}

void graph_to_dot(Graph &graph, ostream &stream) {
    stream << "graph unsat_" << graph.n() << "_" << graph.m() << " {\n";
    stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
    for (int i = 0; i < graph.n(); i++)
        stream << "\t" << i << " [shape=circle];\n";
    for (int i = 0; i < graph.m(); i++)
        stream << "\t" << graph.edges[i][0] << " -- " << graph.edges[i][1] << (graph.edges[i][2] > 0 ? " [style=dashed," : "[") << "]" << ";\n";
    stream << "}";
}

void graph_to_dot_ST(Graph &graph, ostream &stream, vector<bool> &ST) {
    stream << "graph unsat_" << graph.n() << "_" << graph.m() << " {\n";
    stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
    for (int i = 0; i < graph.n(); i++)
        stream << "\t" << i << " [shape=circle];\n";
    for (int i = 0; i < graph.m(); i++)
        stream << "\t" << graph.edges[i][0] << " -- " << graph.edges[i][1] << (ST[i] ? "[style=bold, color=red" : (graph.edges[i][2] > 0 ? " [style=dashed," : "[") )<< "]" << ";\n";
    stream << "}";
}

void graph_to_GAP_command(Graph &graph, ostream &stream) {
    stream << "PrintIsoSignatures([], [";
    for (uint i = 0; i < graph.edges.size(); i++) {
        auto e = graph.edges[i];
        stream << "[" << e[0] + 1 << "," << e[1] + 1 << "]";
        if (i < graph.edges.size() - 1) stream << ",";
    }
    stream << "], 1);\n";
}


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

int n_signatures(Graph &graph) { return (1 << (graph.m() - graph.n() + 1)); }

void next_signature(Graph &graph, vector<bool>& ST) {
    uint i = 0;
    while ((i < graph.m()) && (graph.edges[i][2] == 1 || ST[i])) {
        graph.edges[i][2] = 0;
        i++;
    }
    if (i < graph.edges.size()) graph.edges[i][2] = 1;
}

void graph_to_unsigned_repr(Graph &graph) {
    int e = graph.m();
    for (int i = 0; i < e; i++) {
        auto edge = graph.edges[i];
        if (edge[2]) {
            // negative edge, insert one vertex inbetween
            graph.add_vertex();
            graph.add_edge(edge[0], graph.n()-1);
            graph.add_edge(edge[1], graph.n()-1);
            graph.remove_edge(edge[0], edge[1]);
        }
    }
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

void sat_result_to_coloring(std::vector<int> &sol, std::vector<pair<int, int>> &coloring) {
    int m = (sol.size() - 1) / 6;
    coloring.resize(m, make_pair(2, 2));
    for (int i = 0; i < m; i++) {
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

bool same_base_graph(Graph &g1, Graph &g2) {
    if ((g1.n() == g2.n()) && (g1.m() == g2.m())) {
        for (auto i = 0; i < g1.m(); i++) {
            if (g1.edges[i][0] == g2.edges[i][0] && g1.edges[i][1] == g2.edges[i][1])
                continue;
            return false;
        }
        return true;
    }
    return false;
}

// void solve_each_signature(Graph &graph, bool unsat_only=false, bool print_graph=true) {
//     vector<bool> ST;
//     spanning_tree(graph, ST);
//     vector<pair<int, int>> coloring;
//     int n_signatures = (1 << (graph.m() - graph.n() + 1));
//     int result;
//     cout << "*** NEW GRAPH ***\n";
//     for (int i = 0; i < n_signatures; i++) {
//         result = solve_graph(graph, coloring);
//         if (result != 10) {
//             graph.print();
//             cout << "UNSAT\n";
//         } else if (!unsat_only) {
//             graph.print();
//             cout << "SAT\n";
//             for (auto x : coloring) {
//                 cout << x.first << "," << x.second << " ";
//             }
//             cout << "\n";
//         }
//         next_signature(graph, ST);
//     }
// }

// void solve_single_signature(Graph &graph) {
//     vector<bool> ST;
//     spanning_tree(graph, ST);
//     vector<pair<int, int>> coloring(graph.m(), make_pair(2,2));
//     int result = solve_graph(graph, coloring);
//     if (result != 10) {
//         graph.graph_to_dot(cout);
//     } else {
//         graph.graph_to_dot_colors(cout, coloring);
//     }
//     cout << "// " << (result != 10 ? "UNSAT\n" : "SAT\n");
// }

// void solve_nothing(Graph &graph) {
//     vector<bool> ST;
//     spanning_tree(graph, ST);
//     vector<string> names = {"cub04_1.gv", "cub04_2.gv", "cub04_3.gv", "cub04_4.gv", "cub04_5.gv", "cub04_6.gv", "cub04_7.gv", "cub04_8.gv"};
//     int n_signatures = (1 << (graph.m() - graph.n() + 1));
//     for (int i = 0; i < n_signatures; i++) {
//         ofstream out(names[i]);
//         graph.graph_to_dot_ST(out, ST);
//         next_signature(graph, ST);
//     }
// }
