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

void graph_from_signed_adj_matrix(Graph &graph, vector<vector<int>> &matrix) {
    graph.clear();
    graph.init(matrix.size());
    for (uint i = 0; i < matrix.size(); i++) {
        for (uint j = i+1; j < matrix.size(); j++) {
            if (matrix[i][j]) {
                graph.add_edge(i, j, 1);
            } else if (matrix[j][i]) {
                graph.add_edge(i, j, -1);
            }
        }
    }
}

void graph_to_signed_adj_matrix(Graph &graph, vector<vector<int>> &matrix) {
    matrix.resize(graph.n(), vector<int>(graph.n(), 0));
    for (auto e : graph.edges) {
        if (e[2] == 1) {
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

int graph_from_edge_list(Graph &graph, istream &stream) {
    int n, m;
    if (stream >> n >> m) {
        graph.clear();
        graph.init(n);
        int a, b;
        for (auto i = 0; i < m; i++) {
            stream >> a >> b;
            graph.add_edge(a,b,1);
        }
        return 1;
    }
    return 0;
}

int number_of_triangles(Graph& graph) {
    int t = 0;
    for (int initial = 0; initial < graph.n(); initial++) {
        for (auto neigh : graph.vertex[initial]) {
            for (int common = 0; common < graph.n(); common++) {
                if ((common == initial) || (common == neigh.first)) continue;
                t += ((graph.adj_matrix[initial][common] >= 0) && (graph.adj_matrix[neigh.first][common] >= 0));
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

        for (auto neighbor_pair : graph.vertex[v]) {
            int neighbor = neighbor_pair.first;
            if (!seen[neighbor]) {
                ST[graph.adj_matrix[v][neighbor]] = true;
                Q.push(neighbor);
                seen[neighbor] = true;
            }
        }
    }
}

inline int n_signatures(Graph &graph) { return (1 << (graph.m() - graph.n() + 1)); }

inline int cycle_space_dimension(Graph &graph) {return (graph.m() - graph.n() + 1); }

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
            graph.add_edge(edge[0], graph.n()-1,1);
            graph.add_edge(edge[1], graph.n()-1,1);
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
        for (auto n1_pair : graph.vertex[i]) {
            for (auto n2_pair : graph.vertex[i]) {
                int n1 = n1_pair.first;
                int n2 = n2_pair.first;
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

void generate_cycles(Graph &graph, vector<vector<pair<int, int>>> &result, int max_length) {
    deque<vector<pair<int, int>>> Q;
    // INIT
    for (int i = 0; i < graph.n(); i++ ) {
        Q.push_back(vector<pair<int,int>>(1, make_pair(i, 0)));
    }

    // BFS
    vector<pair<int,int>> potential_cycle;
    int last, first;
    while (!Q.empty()) {
        potential_cycle = Q.front();
        Q.pop_front();
        // looking for cycles of max_length at the longest, we are counting the smallest (first) vertex twice
        if (potential_cycle.size() > max_length + 1) continue;
        first = potential_cycle[0].first;
        last = potential_cycle[potential_cycle.size() - 1].first;
        // each cycle starts with the lowest vertex, this way it gets counted only once and algorithm is faster
        if (last < first) continue;
        if (last == first && potential_cycle.size() > 3) {
            // discard cycles of size 2 and break symmetry
            if (potential_cycle[1].first < potential_cycle[potential_cycle.size() - 2].first) {
                result.push_back(potential_cycle);
            }
            continue;
        }
        // add another vertex and continue
        for (auto neighbor_pair : graph.vertex[last]) {
            int neighbor = neighbor_pair.first;
            int edge = neighbor_pair.second;
            bool breaking = false;
            for (int i = 1; i < potential_cycle.size(); i++) {
                if (potential_cycle[i].first == neighbor) {
                    breaking = true;
                    break;
                }
            }
            if (breaking) continue;
            potential_cycle.push_back(make_pair(neighbor, edge));
            Q.push_back(potential_cycle);
            potential_cycle.pop_back();
         }
    }
}

vector<uint> cycles_to_matrix(vector<vector<pair<int,int>>> cycles) {
    vector<uint> matrix;
    for (auto cycle : cycles) {
        uint row = 0;
        for (int i = 1; i < cycle.size(); i++) {
            row |= (1 << cycle[i].second);
        }
        matrix.push_back(row);
    }
    // for (auto x : matrix) {
    //     cout << x << " , ";
    // }
    // cout << "\n";
    return matrix;
}

void matrix_to_basis(vector<uint> matrix) {
    uint column;
    for (uint i = 0; i < matrix.size(); i++) {
        uint mask = (matrix[i] & (~matrix[i] + 1));
        //cout << matrix[i] << " , " << mask << endl;
        if (!matrix[i]) continue;
        for (uint j = 0; j < matrix.size(); j++) {
            if (j == i) continue;
            if (matrix[j] & mask) {
                matrix[j] ^= matrix[i];
            }
        }
    }
    vector<uint> new_matrix;
    for (auto x : matrix) {
        if (x) {
            new_matrix.push_back(x);
        }
    }
    //cout << new_matrix.size() << endl;
    matrix = new_matrix;
}

void generate_cycle_space(Graph &graph, vector<vector<pair<int,int>>> cycles, vector<uint> cycle_matrix, vector<uint> reduced_cycle_matrix, int max_cycle_length) {
    cycles.clear();

}
