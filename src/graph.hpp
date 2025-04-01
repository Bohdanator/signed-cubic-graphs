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

    inline int n() { return vertex.size(); }
    inline int m() { return edges.size(); }

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

    void remove_edge(int u, int v) {
        int a = min(u, v);
        int b = max(u, v);
        for (int i = 0; i < edges.size(); i++) {
            if (edges[i][0] == a && edges[i][1] == b) {
                edges.erase(edges.begin()+i);
                break;
            }
        }
        adj_matrix[u][v] = -1;
        adj_matrix[v][u] = -1;
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
