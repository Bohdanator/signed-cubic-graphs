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

bool next_signature(Graph &graph, vector<bool>& ST) {
    uint i = 0;
    while ((i < graph.m()) && (graph.edges[i][2] == -1 || ST[i])) {
        graph.edges[i][2] = 1;
        i++;
    }
    if (i < graph.edges.size()) {
        graph.edges[i][2] = -1;
    }
    return i < graph.m();
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

void generate_cycles_with_cache(Graph &G, vector<Cycle> &C, vector<Cycle> cache, int max_length) {
    deque<Cycle> Q;
    while (!cache.empty()) {
        Q.push_back(cache[cache.size() - 1]);
        cache.pop_back();
    }
    // BFS
    int last, first;
    while (!Q.empty()) {
        Cycle potential_cycle = Q.front();
        Q.pop_front();
        // looking for cycles of max_length at the longest
        if (potential_cycle.length() > max_length) {
            cache.push_back(potential_cycle);
            continue;
        }
        first = potential_cycle.first_vertex();
        last = potential_cycle.last_vertex();
        // each cycle starts with the lowest vertex, this way it gets counted only once and algorithm is faster
        if (last < first) continue;
        if (potential_cycle.complete() && potential_cycle.length() > 2) {
            // discard cycles of size 2 and break symmetry
            if (potential_cycle.symmetrically_lowest()) {
                C.push_back(potential_cycle);
            }
            continue;
        }
        // add another vertex and continue
        for (auto neighbor_pair : G.vertex[last]) {
            int neighbor = neighbor_pair.first;
            int edge = neighbor_pair.second;
            int sign = G.sign_of_edge(edge);
            if (neighbor < first) continue;
            bool breaking = false;
            // iterate the actual representation
            for (int i = 1; i < potential_cycle.cycle.size(); i++) {
                if (potential_cycle.vertex_at(i) == neighbor) {
                    breaking = true;
                    break;
                }
            }
            if (breaking) continue;
            potential_cycle.add(neighbor, edge, sign);
            Q.push_back(potential_cycle);
            potential_cycle.remove_last();
        }
    }
}

void generate_cycles(Graph &graph, vector<Cycle> &result, int max_length) {
    deque<Cycle> Q;
    // INIT
    for (int i = 0; i < graph.n(); i++ ) {
        Q.push_back(Cycle(i));
    }

    // BFS
    int last, first;
    while (!Q.empty()) {
        Cycle potential_cycle = Q.front();
        Q.pop_front();
        // looking for cycles of max_length at the longest
        if (potential_cycle.length() > max_length) continue;
        first = potential_cycle.first_vertex();
        last = potential_cycle.last_vertex();
        // each cycle starts with the lowest vertex, this way it gets counted only once and algorithm is faster
        if (last < first) continue;
        if (potential_cycle.complete() && potential_cycle.length() > 2) {
            // discard cycles of size 2 and break symmetry
            if (potential_cycle.symmetrically_lowest()) {
                result.push_back(potential_cycle);
            }
            continue;
        }
        // add another vertex and continue
        for (auto neighbor_pair : graph.vertex[last]) {
            int neighbor = neighbor_pair.first;
            int edge = neighbor_pair.second;
            int sign = graph.sign_of_edge(edge);
            if (neighbor < first) continue;
            bool breaking = false;
            // iterate the actual representation
            for (int i = 1; i < potential_cycle.cycle.size(); i++) {
                if (potential_cycle.vertex_at(i) == neighbor) {
                    breaking = true;
                    break;
                }
            }
            if (breaking) continue;
            potential_cycle.add(neighbor, edge, sign);
            Q.push_back(potential_cycle);
            potential_cycle.remove_last();
         }
    }
}

vector<uint> cycles_to_matrix(vector<Cycle> &cycles) {
    vector<uint> matrix;
    for (uint j = 0; j < cycles.size(); j++) {
        Cycle cycle = cycles[j];
        uint row = 0;
        for (int i = 1; i < cycle.cycle.size(); i++) {
            row |= (1 << cycle.cycle[i][1]);
        }
        matrix.push_back(row);
    }
    // for (auto x : matrix) {
    //     cout << x << " , ";
    // }
    // cout << "\n";
    return matrix;
}

void matrix_to_basis(vector<uint> &matrix) {
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

vector<uint> generate_cycles_with_coverage(Graph &G, vector<Cycle> &C, int &max_length) {
    vector<Cycle> cache;
    for (int i = 0; i < G.n(); i++ ) {
        cache.push_back(Cycle(i));
    }
    bool covered = false;
    max_length = 3;
    vector<uint> matrix;
    while (!covered) {
        generate_cycles_with_cache(G, C, cache, max_length);
        matrix = cycles_to_matrix(C);
        matrix_to_basis(matrix);
        covered = (matrix.size() >= cycle_space_dimension(G));
        if (!covered) max_length++;
    }
    return matrix;
}

// for each cycle add a new cycle vertex connected to each vertex of the cycle
// cycle vertex has one tail if cycle is unbalanced and two tails if cycle is balanced
void add_cycle_vertices(Graph &graph, vector<Cycle> &cycles) {
    for (auto cycle : cycles) {
        int cycle_vertex = graph.n();
        graph.add_vertex();
        // connect new vertex to vertices of the cycle
        for (int i = 0; i < cycle.length(); i++) {
            graph.add_edge(cycle.vertex_at(i), cycle_vertex, 1);
        }
        // add tails
        if (cycle.cycle_sign == 1) {
            graph.add_vertex();
            graph.add_edge(cycle_vertex, cycle_vertex + 1, 1);
        } else {
            graph.add_vertex();
            graph.add_edge(cycle_vertex, cycle_vertex + 1, 1);
            graph.add_vertex();
            graph.add_edge(cycle_vertex, cycle_vertex + 2, 1);
        }
    }
}

// compute which cycles is given vertex present
void edge_cycle_mapping(vector<Cycle> &cycles, vector<vector<int>> &mapping, int m) {
    mapping.resize(m);
    for (uint cycle_index = 0; cycle_index < cycles.size(); cycle_index++) {
        Cycle cycle = cycles[cycle_index];
        for (uint i = 1; i < cycle.cycle.size(); i++) {
            mapping[cycle.cycle[i][1]].push_back(cycle_index);
        }
    }
}

void switch_edge_and_cycles(int e, Graph &graph, vector<Cycle> &cycles, vector<vector<int>> &mapping) {
    graph.edges[e][2] *= -1;
    for (auto c : mapping[e]) {
        cycles[c].cycle_sign *= -1;
    }
}

bool next_signature_cycles(Graph &graph, vector<Cycle> &cycles, vector<bool> &ST, vector<vector<int>> &mapping) {
    uint i = 0;
    while ((i < graph.m()) && (graph.edges[i][2] == -1 || ST[i])) {
        if (graph.edges[i][2] == -1) switch_edge_and_cycles(i, graph, cycles, mapping);
        i++;
    }
if (i < graph.edges.size() && !ST[i]) {
        switch_edge_and_cycles(i, graph, cycles, mapping);
    }
    return i < graph.m();
}
