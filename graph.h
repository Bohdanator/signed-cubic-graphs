#include<vector>
#include<string>
#include<iostream>
#include<queue>

using namespace std;

struct Graph {
    vector<vector<int>> vertex; // <neighbor, edge>
    vector<vector<int>> edges; // [u, v, sign, color]
    vector<vector<int>> adj_matrix;

    Graph() = default;

    Graph(int n) {
        vertex = vector<vector<int>>(n, vector<int>(0));
        adj_matrix = vector<vector<int>>(n, vector<int>(n, -1));
        edges = vector<vector<int>>(0);
    }

    int n() { return vertex.size(); }

    int m() { return edges.size(); }

    void print() {
        cout << vertex.size() << " " << edges.size() << "\n";
        for (int i = 0; i < edges.size(); i++) {
            for (int j = 0; j < edges[i].size(); j++) {
                cout << edges[i][j] << ",";
            }
            cout << " ";
        }
        cout << "\n";
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

    void parse_from_stream(istream &stream) {
        string buf;
        int n, m;
        string u, v, s;

        stream >> n >> m;
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
    }

    void signature(vector<int> &dest) {
        dest.resize(edges.size());
        for (int i = 0; i < edges.size(); i++) {
            dest[i] = edges[i][2];
        }
    }

    void colors(vector<int> &dest) {
        dest.resize(edges.size());
        for (int i = 0; i < edges.size(); i++) {
            dest[i] = edges[i][3];
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
