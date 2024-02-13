#include<bits/stdc++.h>

using namespace std;

string SAT_INSTANCE_FILENAME = "sat_file.dimacs";
string OUTPUT_FILENAME = "output.txt";
string SAT_SOLVER_PATH = "kissat-3.1.0-linux-amd64";

struct Graph {
    int v;
    vector<vector<int>> A;
    vector<vector<int>> N;
    vector<pair<int, int>> E;

    Graph() = default;

    Graph(int n) : v(n) {
        A = vector<vector<int>>(n, vector<int>(n, -1));
        N = vector<vector<int>>(n, vector<int>(0));
    }

    void print_e() {
        cout << v << " " << E.size() << "\n";
        for (auto p : E){
            cout << p.first << "," << p.second << " ";
        }
        cout << endl;
    }

    void print_am() {
        cout << v << " " << E.size() << "\n";
        for (auto row : A) {
            for (auto x : row) {
                cout << x << " ";
            }
            cout << "\n";
        }
    }

    void add_edge(int a, int b) {
        if (A[a][b] >= 0) return;
        A[a][b] = E.size();
        A[b][a] = E.size();
        N[a].push_back(b);
        N[b].push_back(a);
        E.push_back(make_pair(a, b));
    }
};

int number_of_triangles(Graph& graph) {
    int t = 0;
    for (int initial = 0; initial < graph.v; initial++) {
        for (int neigh : graph.N[initial]) {
            for (int common = 0; common < graph.v; common++) {
                if ((common == initial) || (common == neigh)) continue;
                t += ((graph.A[initial][common] >= 0) && (graph.A[neigh][common] >= 0));
            }
        }
    }
    return t / 6;
}

void spanning_tree(Graph& graph, vector<bool>& ST) {
    ST.resize(graph.E.size(), false);
    queue<int> Q;
    vector<bool> seen(graph.v, 0);
    Q.push(0);
    int v;
    while(!Q.empty()) {
        v = Q.front();
        Q.pop();
        seen[v] = true;

        for (auto neighbor : graph.N[v]) {
            if (!seen[neighbor]) {
                ST[graph.A[v][neighbor]] = true;
                Q.push(neighbor);
                seen[neighbor] = true;
            }
        }
    }
}

void graph_to_SAT(Graph graph, vector<int> &signature, vector<vector<int>> &sat) {
    // colors: 3i + 1 = +1; 3i + 2 = -1
    // half-edges (u,v); u < v -> 6i+1 = u_start; 6i+4 = v_start
    // edge constraints
    sat.clear();
    for (int i = 0; i < graph.E.size(); i++) {
        // two half-edges, each has at least one color
        sat.push_back({6*i+1, 6*i+2, 6*i+3});
        sat.push_back({6*i+4, 6*i+5, 6*i+6});
        // signature constraints for each half-edge
        if (signature[i] == 1) {
            sat.push_back({(-1)*(6*i+1), (-1)*(6*i+4)});
            sat.push_back({(-1)*(6*i+2), (-1)*(6*i+5)});
        } else {
            sat.push_back({(-1)*(6*i+1), (-1)*(6*i+5)});
            sat.push_back({(-1)*(6*i+2), (-1)*(6*i+4)});
        }
        // color constraints for each half-edge
        sat.push_back({(-1)*(6*i+1), (-1)*(6*i+6)});
        sat.push_back({(-1)*(6*i+2), (-1)*(6*i+6)});
        sat.push_back({(-1)*(6*i+3), (-1)*(6*i+4)});
        sat.push_back({(-1)*(6*i+3), (-1)*(6*i+5)});
    }
    // different colors for each vertex constraints
    // each pair of half-edges at a vertex is different
    for (int i = 0; i < graph.v; i++) {
        for (auto n1 : graph.N[i]) {
            for (auto n2 : graph.N[i]) {
                if (n1 == n2) continue;
                // starting colors
                int e1 = 6*graph.A[i][n1] + (n1 > i ? 4 : 1);
                int e2 = 6*graph.A[i][n2] + (n2 > i ? 4 : 1);
                for (int j = 0; j < 3; j++) {
                    sat.push_back({-(e1+j), -(e2+j)});
                }
            }
        }
    }
}

void SAT_to_coloring(vector<bool> &sat_result, vector<int> &colors) {
    colors.resize(sat_result.size()/3, 0);
    for (int i = 0; i < sat_result.size()-3; i+=3) {
        if (sat_result[i]) {
            colors[i] = -1;
        }
        else if (sat_result[i+1]) {
            colors[i] = 1;
        }
        else if(sat_result[i=2]) {
            colors[i] = 0;
        }
        else {
            colors[i] = 2;
        }
    }
}

bool read_SAT_result(ifstream &file, vector<bool> &result) {
    string buf;
    if (!getline(file, buf)) return false;
    if (buf != "s SATISFIABLE") return false;
    result.clear();
    while(getline(file, buf)) {
        stringstream ss(buf);
        string var;
        while(ss >> var) {
            int v = stoi(var);
            if (v == 0) {
                return true;
            } else if (v > 0) {
                result.push_back(true);
            } else {
                result.push_back(false);
            }
        }
    }
}

void write_SAT_instance(string filename, int n_vars, const vector<vector<int>> &sat) {
    cout << filename << endl;
    ofstream out(filename);
    out << "p cnf " << n_vars << " " << sat.size() << "\n";
    for (auto formula : sat) {
        for (auto x : formula) {
            out << x << " ";
        }
        out << "0\n";
    }
    out.close();
}

void solve_SAT_instance(string in_filename, string sat_path) {
    int pid = fork();
    if (pid == 0) {
        cout << "start solving\n";
        char* args[] = {sat_path.data(), "-q", in_filename.data(), NULL};
        execv(sat_path.c_str(), args);
    }
}

void next_signature(Graph graph, vector<bool> &ST, vector<int> &signature) {
    int i = 0;
    while ((i < signature.size()) && (signature[i] == 1 || ST[i])) {
        signature[i] = 0;
        i++;
    }
    if (i < signature.size()) signature[i] = 1;
}

void print_signature(const vector<int> &signature) {
    for (auto x : signature) {
        cout << x << " ";
    }
    cout << "\n";
}

Graph load_graph(ifstream& fin) {
    string buf;

    getline(fin, buf);
    int n = buf.size();
    Graph graph(n);
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < buf.size(); c++) {
            if (buf[c] == '1' && r <= c) {
                graph.add_edge(r, c);
            }
        }
        getline(fin, buf);
    }
    return graph;
}

int main(int argc, char** argv) {
    string filename, graphs_path;
    string buf, w;
    Graph graph;
    if (argc > 1) {
        filename = argv[1];
        graphs_path = "graph-data/" + filename;
    } else {
        "The first argument is the file containing graphs.\n";
        return 1;
    }
    // load graphs
    ifstream fin(graphs_path);
    int graph_n = 0;
    getline(fin, buf);
    while(getline(fin, buf)) {
        // for each graph
        stringstream ss(buf);
        getline(ss, w, ' ');
        // check format
        if (w != "Graph") {
            break;
        }
        // load graph data
        graph = load_graph(fin);
        graph.print_e();
        // make the spanning tree
        vector<bool> ST(false, graph.v);
        spanning_tree(graph, ST);
        for (auto x : ST) cout << x << endl;
        // iterate signatures
        int n_signatures = (1 << (graph.E.size() - graph.v + 1));
        vector<int> signature(graph.E.size(), 0);
        vector<vector<int>> sat_formulas;
        cout << "starting signatures\n";
        for (int i = 0; i < n_signatures; i++) {
            // create SAT formulas for current signature
            graph_to_SAT(graph, signature, sat_formulas);
            // write SAT instance
            write_SAT_instance("sat-data/"+filename+"-"+to_string(graph_n)+"-"+to_string(i), 6*graph.E.size(), sat_formulas);
            // next signature
            next_signature(graph, ST, signature);
            print_signature(signature);
            cout << "done\n";
        }
        graph_n++;
    }
}
