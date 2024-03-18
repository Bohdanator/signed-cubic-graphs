#include<bits/stdc++.h>
#include "graph.h"

using namespace std;

void solve_each_signature(Graph &graph, bool unsat_only=false, bool print_graph=true) {
    vector<bool> ST;
    spanning_tree(graph, ST);
    vector<pair<int, int>> coloring;
    int n_signatures = (1 << (graph.m() - graph.n() + 1));
    int result;
    cout << "*** NEW GRAPH ***\n";
    for (int i = 0; i < n_signatures; i++) {
        result = solve_graph(graph, coloring);
        if (result != 10) {
            graph.print();
            cout << "UNSAT\n";
        } else if (!unsat_only) {
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

void solve_single_signature(Graph &graph) {
    vector<bool> ST;
    spanning_tree(graph, ST);
    vector<pair<int, int>> coloring(graph.m(), make_pair(2,2));
    int result = solve_graph(graph, coloring);
    if (result != 10) {
        graph.graph_to_dot(cout);
    } else {
        graph.graph_to_dot_colors(cout, coloring);
    }
    cout << "// " << (result != 10 ? "UNSAT\n" : "SAT\n");
}

void solve_nothing(Graph &graph) {
    vector<bool> ST;
    spanning_tree(graph, ST);
    vector<string> names = {"cub04_1.gv", "cub04_2.gv", "cub04_3.gv", "cub04_4.gv", "cub04_5.gv", "cub04_6.gv", "cub04_7.gv", "cub04_8.gv"};
    int n_signatures = (1 << (graph.m() - graph.n() + 1));
    for (int i = 0; i < n_signatures; i++) {
        ofstream out(names[i]);
        graph.graph_to_dot_ST(out, ST);
        next_signature(graph, ST);
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        string filename = argv[1];
        ifstream in(filename);
        while(in.eofbit) {
            Graph graph;
            graph.parse_from_stream(in);
            solve_each_signature(graph, true);
        }
    } else {
        while(cin.eofbit) {
            Graph graph;
            graph.parse_from_stream(cin);
            solve_single_signature(graph);
            //solve_nothing(graph);
        }
    }
}
