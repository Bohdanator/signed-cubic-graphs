#include<iostream>
#include<fstream>
#include<syncstream>
#include<vector>
#include<string>
#include<thread>
#include<mutex>
#include "sat/kissat_wrapper.hpp"
#include "visualization.hpp"
#include "isomorphism_utils.hpp"

using namespace std;

mutex stream_lock;

string root_dir = "dumps/";
string vertex_dir = "results/";

string strong_snarks_fn = "strong_snarks.txt";
string strong_colorable_fn = "strong_colorable.txt";
string signed_snarks_fn = "signed_snarks.txt";
string stats_fn = "stats.txt";

ofstream strong_snarks;
ofstream strong_colorable;
ofstream signed_snarks;
ofstream stats;

int processed_graphs = 0;
int PROCESSED_MOD = 500;

class SolverThread
{
public:
    Graph base;
    vector<Graph> non_isomorphic;
    vector<pair<Graph, vector<pair<int,int>>>> colorable;
    vector<Graph> snarks;
    int n_non_isomorphic;

    SolverThread() = default;

    void operator() () {

        while (1) {
            {
                lock_guard<mutex> lock(stream_lock);
                if (!load_graph()) break;
            }
            {
                // generate non-isomorphic graphs
                non_isomorphic.clear();
                n_non_isomorphic = generate_non_isomorphic_graphs(base, non_isomorphic);
                // coloring
                colorable.clear();
                snarks.clear();
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
                        colorable.push_back(make_pair(unique_graph, coloring));
                    } else {
                        snarks.push_back(unique_graph);
                    }
                }
            }
            {
                lock_guard<mutex> lock(stream_lock);
                print_results();
            }
        }
    }

    bool load_graph() {
        base.clear();
        return parse_graph(base, cin);
    }

    void print_results() {
        print_graph(base, stats);
        stats << "non_isomorphic " << n_non_isomorphic << "\ncolorable " << colorable.size() << "\nsnarks " << snarks.size() << "\n";
        if (snarks.size() == 0) {
            print_graph(base, strong_colorable);
        }
        if (colorable.size() == 0) {
            print_graph(base, strong_snarks);
        }
        for (auto g : snarks) {
            print_graph(g, signed_snarks);
        }
        if (++processed_graphs % PROCESSED_MOD == 0) {
            cout << "checkpoint: " << processed_graphs << " graphs done\n";
        }
    }
};

int main(int argc, char** argv) {

    if (argc > 1) {
        vertex_dir = argv[1];
    }
    int n_threads = thread::hardware_concurrency();
    if (argc > 2) n_threads = stoi(argv[2]);
    int to_skip = 0;
    if (argc > 3) to_skip = stoi(argv[3]);

    strong_snarks.open(root_dir + vertex_dir + strong_snarks_fn, ofstream::app);
    strong_colorable.open(root_dir + vertex_dir + strong_colorable_fn, ofstream::app);
    signed_snarks.open(root_dir + vertex_dir + signed_snarks_fn, ofstream::app);
    stats.open(root_dir + vertex_dir + stats_fn, ofstream::app);

    vector<SolverThread> solvers(n_threads);
    cout << "Starting computation on " << n_threads << " threads.\n";
    cout << "Skipping " << to_skip << " graphs... ";
    Graph buffer;
    while(to_skip-- > 0) {
        parse_graph(buffer, cin);
    }
    cout << "Done.\n";
    cout.flush();
    vector<thread> threads;
    for (size_t i = 0; i < n_threads; i++) {
        lock_guard<mutex> lock(stream_lock);
        threads.push_back(thread(solvers[i]));
    }
    for (size_t i = 0; i < n_threads; i++) {
        threads[i].join();
        lock_guard<mutex> lock(stream_lock);
        cout << "joined thread\n";
    }
    cout << "Done.\n";
}
