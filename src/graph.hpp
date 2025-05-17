#ifndef GRAPH_HPP
#define GRAPH_HPP

#include<vector>
#include<string>
#include<iostream>
#include<queue>

using namespace std;

class Graph {
public:
    vector<vector<pair<int,int>>> vertex; // [neighbor, edge]
    vector<vector<int>> edges; // [u, v, sign]
    vector<vector<int>> adj_matrix;

    Graph() = default;

    // initialize empty graph with n vertices
    void init(int n) {
        vertex = vector<vector<pair<int,int>>>(n, vector<pair<int,int>>(0));
        adj_matrix = vector<vector<int>>(n, vector<int>(n, 0));
        edges = vector<vector<int>>(0);
    }
    // clear graph
    void clear() {
        vertex.clear();
        edges.clear();
        adj_matrix.clear();
    }

    inline size_t n() { return vertex.size(); }
    inline size_t m() { return edges.size(); }

    void add_vertex() {
        vertex.push_back({});
        for (int i = 0; i < adj_matrix.size(); i++) {
            adj_matrix[i].push_back(0);
        }
        adj_matrix.push_back(vector<int>(vertex.size(), -1));
    }

    inline int sign_of_edge(int e) {
        return edges[e][2];
    }

    inline int sign_of_edge(int u, int v) {
        return edges[adj_matrix[u][v]][2];
    }

    void add_edge(int u, int v, int sign) {
        if (adj_matrix[u][v]) return;
        vector<int> e = {u, v, sign};
        vertex[e[0]].push_back(make_pair(e[1],edges.size()));
        vertex[e[1]].push_back(make_pair(e[0],edges.size()));
        adj_matrix[e[0]][e[1]] = edges.size();
        adj_matrix[e[1]][e[0]] = edges.size();
        edges.push_back(e);
    }

    void signature(vector<int> &dest) {
        dest.resize(edges.size());
        for (uint i = 0; i < edges.size(); i++) {
            dest[i] = edges[i][2];
        }
    }

    void operator=(Graph &other) {
        vertex = other.vertex;
        edges = other.edges;
        adj_matrix = other.adj_matrix;
    }
};

class Cycle {
public:
    vector<vector<int>> cycle; // [vertex, edge, sign]
    int cycle_sign;

    Cycle() {
        cycle_sign = 1;
    };

    Cycle(int initial_vertex) {
        cycle_sign = 1;
        add(initial_vertex, -1, 1);
    }

    inline int first_vertex() {
        return cycle[0][0];
    }

    inline int last_vertex() {
        return cycle[cycle.size() - 1][0];
    }

    inline size_t vertex_at(int index) {
        return cycle[index][0];
    }

    inline bool complete() {
        return first_vertex() == last_vertex();
    }

    // breaking symmetry by starting the cycle at the lowest vertex (rotation) and
    // having the second vertex smaller than second to last vertex (direction)
    inline bool symmetrically_lowest() {
        return vertex_at(1) < vertex_at(length() - 1);
    }

    // vertex at length() is the same as vertex at 0 in a complete cycle
    inline size_t length() {
        return cycle.size() - 1;
    }

    void add(int vertex, int edge, int sign) {
        vector<int> vec = {vertex, edge, sign};
        cycle.push_back(vec);
        cycle_sign *= sign;
    }

    void remove_last() {
        vector<int> last = cycle[cycle.size() - 1];
        cycle_sign *= last[2];
        cycle.pop_back();
    }
};


#endif
