#include<iostream>
#include<fstream>
#include<syncstream>
#include<vector>
#include<string>
#include "sat/kissat_wrapper.hpp"
#include "visualization.hpp"
#include "isomorphism_utils.hpp"

using namespace std;

int PROCESSED_MOD = 500;

void solve_graph(Graph &graph, vector<Graph> &non_isomorphic, vector<Graph> &snarks) {
    non_isomorphic.clear();
    snarks.clear();
    int n_iso = generate_non_isomorphic_graphs(graph, non_isomorphic);

    for (auto unique_graph : non_isomorphic) {
        vector<vector<int>> sat_instance;
        vector<int> sol;
        KissatWrapper sat_solver;
        graph_to_SAT(unique_graph, sat_instance);
        int result = sat_solver.solve(sat_instance, sol);
        if (result == 10) {
            // coloring exists
            vector<pair<int, int>> coloring;
            sat_result_to_coloring(sol, coloring);
        } else {
            snarks.push_back(unique_graph);
        }
    }
}

int main(int argc, char** argv) {
    string stats_fname = argv[1];
    string refined_fname = argv[2];
    string snarks_fname = argv[3];
    ifstream stats("dumps/" + stats_fname);
    ofstream refined("dumps/refined/" + refined_fname);
    ofstream snarks_stream("dumps/" + snarks_fname, ostream::app);

    vector<Graph> all;
    Graph buf;
    while(parse_graph(buf, cin)) {
        all.push_back(buf);
        if(all.size() % PROCESSED_MOD == 0) {
            cout << "checkpoint, loaded " << all.size() << " graphs\n";
        }
    }
    cout << "parsing all graphs done\n";

    vector<bool> processed(all.size(), false);
    vector<Graph> snarks;
    vector<Graph> non_iso;
    string word;
    int n_non_iso, n_colorable, n_snarks;
    bool cont;
    int done = 0;
    while(parse_graph(buf, stats)) {
        if(++done % PROCESSED_MOD == 0) {
            cout << "checkpoint, processed " << done << " graphs\n";
        }

        stats >> word >> n_non_iso;
        stats >> word >> n_colorable;
        stats >> word >> n_snarks;

        cont = false;
        for (int i = 0; i < all.size(); i++) {
            if (same_base_graph(all[i], buf)) {
                if (processed[i]) {
                    cout << "already processed graph\n";
                    cont = true;
                    break;
                } else {
                    processed[i] = true;
                }
            }
        }
        if (cont) {
            continue;
        }

        print_graph(buf, refined);
        refined << "n " << n_non_iso << "\n";
        refined << "s " << n_snarks << "\n";
    }
    cout << "parsing processed graphs done\n";

    for (int i = 0; i < all.size(); i++) {
        if (!processed[i]) {
            cout << "not processed graph\n";
            print_graph(all[i], cout);
            solve_graph(all[i], non_iso, snarks);
            print_graph(all[i], refined);
            for (auto snark : snarks) {
                print_graph(snark, snarks_stream);
            }
            refined << "n " << non_iso.size() << "\n";
            refined << "s " << snarks.size() << "\n";
        }
    }
}
