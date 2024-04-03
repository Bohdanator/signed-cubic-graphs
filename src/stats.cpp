#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include "graph.hpp"
#include "graph_utils.hpp"
#include "sat/sat_solver_wrapper.hpp"
#include "sat/cadical_wrapper.hpp"
#include "sat/kissat_wrapper.hpp"

using namespace std;

int solve_graph(Graph &graph, std::vector<pair<int, int>> &coloring) {
    std::vector<std::vector<int>> sat_instance;
    graph_to_SAT(graph, sat_instance);

    std::vector<int> sol;
    KissatWrapper solver;
    int result = solver.solve(sat_instance, sol);
    if (result != 10) {
        return result;
    }
    sat_result_to_coloring(sol, coloring);
    return result;
}

int main(int argc, char** argv) {
    if (argc != 5) return 0;
    string fin = argv[1];
    string snarks_fn = argv[2];
    string regular_fn = argv[3];
    string colorable_fn = argv[4];
    ifstream in(fin);
    ofstream snarks_out(snarks_fn);//, ofstream::ate);
    ofstream colorable_out(colorable_fn);//, ofstream::ate);
    // ofstream regular_out(regular_fn);//, ofstream::ate);

    Graph biggest;
    int n_biggest = 0;
    int n_snarks = 0;
    int n_all = 0;
    int n_graphs = 0;

    Graph graph;
    while(graph_from_edge_list(graph, in)) {
        //cout << "New graph " << n_all << "\n";
        vector<bool> ST;
        spanning_tree(graph, ST);

        vector<pair<int, int>> coloring;
        int snarks = 0;
        int n_signatures = (1 << (graph.m() - graph.n() + 1));
        for(int i = 0; i < n_signatures; i++) {
            int result = solve_graph(graph, coloring);
            if (result == 10) {
                // SAT
                // graph.print(regular_out);
                // for (auto x : coloring) {
                //     regular_out << x.first << "," << x.second << " ";
                // }
                // regular_out << "\n";
            } else{
                // UNSAT
                print_graph(graph,snarks_out);
                snarks++;
            }
            next_signature(graph, ST);
        }
        if (snarks == 0) {
            print_graph(graph,colorable_out);
        }
        if (snarks > n_biggest) {
            n_biggest = snarks;
            biggest = graph;
        }
        n_snarks += snarks;
        n_all += n_signatures;
        n_graphs++;

        snarks_out.flush();
        colorable_out.flush();
        //regular_out.flush();
        graph.clear();
    }

    cout << n_graphs << " " << n_all << " " << n_snarks << endl;
    cout << (1 << (biggest.m() - biggest.n() + 1)) << " " << n_biggest << endl;
    print_graph(biggest, cout);
    snarks_out.close();
    //regular_out.close();
    return 0;
}
