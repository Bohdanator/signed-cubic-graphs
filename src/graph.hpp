#ifndef GRAPH_HPP
#define GRAPH_HPP

#include<vector>
#include<string>
#include<iostream>
#include<queue>

using namespace std;

class Graph {
public:
    vector<vector<int>> vertex; // [neighbor, edge]
    vector<vector<int>> edges; // [u, v, sign]
    vector<vector<int>> adj_matrix;

    Graph() = default;

    // initialize empty graph with n vertices
    void init(int n) {
        vertex = vector<vector<int>>(n, vector<int>(0));
        adj_matrix = vector<vector<int>>(n, vector<int>(n, -1));
        edges = vector<vector<int>>(0);
    }
    // clear graph
    void clear() {
        vertex.clear();
        edges.clear();
        adj_matrix.clear();
    }

    inline int n() { return vertex.size(); }
    inline int m() { return edges.size(); }

    void print(ostream &out = cout) {
        out << vertex.size() << " " << edges.size() << "\n";
        for (uint i = 0; i < edges.size(); i++) {
            for (uint j = 0; j < edges[i].size(); j++) {
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


#endif
