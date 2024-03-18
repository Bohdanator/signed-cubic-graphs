#include<bits/stdc++.h>
#include "graph.h"

int main(int argc, char** argv) {
    if (argc != 4) return 0;
    string fin = argv[1];
    string snarks_fn = argv[2];
    string regular_fn = argv[3];
    ifstream in(fin);
    ofstream snarks_out(snarks_fn, ofstream::ate);
    ofstream regular_out(regular_fn, ofstream::ate);

    Graph biggest;
    int n_biggest = 0;
    int n_snarks = 0;
    int n_all = 0;
    int n_graphs = 0;

    Graph graph;
    while(graph.parse_from_stream(in)) {
        cout << "New graph " << n_all << "\n";
        vector<bool> ST;
        spanning_tree(graph, ST);

        vector<pair<int, int>> coloring;
        int snarks = 0;
        int n_signatures = (1 << (graph.m() - graph.n() + 1));
        for(int i = 0; i < n_signatures; i++) {
            int result = solve_graph(graph, coloring);
            if (result == 10) {
                // SAT
                graph.print(regular_out);
                for (auto x : coloring) {
                    regular_out << x.first << "," << x.second << " ";
                }
                regular_out << "\n";
            } else{
                // UNSAT
                graph.print(snarks_out);
                snarks++;
            }
            next_signature(graph, ST);
        }

        if (snarks > n_biggest) {
            n_biggest = snarks;
            biggest = graph;
        }
        n_snarks += snarks;
        n_all += n_signatures;
        n_graphs++;

        snarks_out.flush();
        regular_out.flush();
        graph.clear();
    }

    cout << n_graphs << " " << n_all << " " << n_snarks << endl;
    cout << (1 << (biggest.m() - biggest.n() + 1)) << " " << n_biggest << endl;
    biggest.print();
    snarks_out.close();
    regular_out.close();
    return 0;
}
