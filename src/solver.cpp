#include<iostream>
#include<fstream>
#include<syncstream>
#include<vector>
#include<string>
#include<thread>
#include<mutex>
#include "sat/kissat_wrapper.hpp"
#include "visualization.hpp"
#include "graph_utils.hpp"

#include "nauty/nauty.h"
#include "nauty/gtools.h"

using namespace std;

mutex stream_lock;

string root_dir = "dumps/";
string vertex_dir = "results/";

string strong_snarks_fn = "strong_snarks.txt";
string strong_colorable_fn = "strong_colorable.txt";
string signed_snarks_fn = "signed_snarks.txt";
string signed_colorable_fn = "signed_colorable.txt";
string stats_fn = "stats.txt";

ofstream strong_snarks;
ofstream strong_colorable;
ofstream signed_snarks;
ofstream signed_colorable;
ofstream stats;
ofstream log;

int processed_graphs = 0;
int PROCESSED_MOD = 500;

class SolverThread
{
public:
    SolverThread() = default;

    void operator() () {
        Graph base;
        vector<Graph> non_isomorphic;
        vector<Cycle> cycles;
        vector<bool> ST;
        vector<vector<int>> mapping;
        vector<pair<Graph, vector<pair<int,int>>>> colorable;
        vector<Graph> snarks;
        int n_non_isomorphic;
        int max_length;

        // DYNALLSTAT(graph, nauty_graph, g_sz);
        // DYNALLSTAT(graph, canon, canon_sz);
        // DYNALLSTAT(int, lab, lab_sz);
        // DYNALLSTAT(int, ptn, ptn_sz);
        // DYNALLSTAT(int, orbits, orbits_sz);

        static DEFAULTOPTIONS_GRAPH(options);
        options.getcanon = true;
        options.schreier = true;
        statsblk nauty_stats;

        vector<graph*> canons;
        size_t n_canons, k, c;
        bool computing = true;

        while (1) {
            {
                lock_guard<mutex> lock(stream_lock);
                base.clear();
                if(!parse_graph(base, cin)) break;
            }
            {
                non_isomorphic.clear();

                spanning_tree(base, ST);
                generate_cycles_with_coverage(base, cycles, max_length);
                edge_cycle_mapping(cycles, mapping, base.m());

                int n = base.n() + 3*cycles.size() + 3;
                int m = SETWORDSNEEDED(n);
                nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

                // cout << "alloc...";
                // DYNALLOC2(graph,nauty_graph,g_sz,m,n,"malloc");
                // DYNALLOC2(graph,canon,canon_sz,m,n,"malloc");
                // DYNALLOC1(int,lab,lab_sz,n,"malloc");
                // DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
                // DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
                // cout << "done\n";

                graph nauty_graph[m*(size_t)n];
                graph canon[m*(size_t)n];
                int lab[n];
                int ptn[n];
                int orbits[n];

                uint cycle_vertex;
                Cycle cycle;
                // reconstruct base graph
                EMPTYGRAPH(nauty_graph,m,n);
                for (auto edge : base.edges) {
                    ADDONEEDGE(nauty_graph, edge[0], edge[1], m);
                }
                for (size_t i = 0; i < cycles.size(); i++) {
                    cycle_vertex = base.n() + 3*i;
                    cycle = cycles[i];
                    for (size_t j = 0; j < cycle.length(); j++) {
                        ADDONEEDGE(nauty_graph, cycle_vertex, cycle.vertex_at(j), m);
                    }
                    // first tail is always connected, second represents the balance
                    ADDONEEDGE(nauty_graph, cycle_vertex, cycle_vertex + 1, m);
                }

                computing = true;
                n_canons = 0;
                canons.clear();
                while (computing) {
                    for (uint cind = 0; cind < cycles.size(); cind++) {
                        int cycle_vertex = base.n() + 3*cind;
                        if (cycles[cind].cycle_sign  == -1) {
                            ADDONEEDGE(nauty_graph, cycle_vertex + 1, cycle_vertex + 2, m);
                        }
                    }

                    densenauty(nauty_graph, lab, ptn, orbits, &options, &nauty_stats, m, n, canon);

                    for (c = 0; c < n_canons; c++) {
                        for (k = 0; k < m*(size_t)n; k++) {
                            if (canons[c][k] != canon[k]) {
                                break;
                            }
                        }
                        if (k >= m*(size_t)n) {
                            // isomorphic with a previous graph
                            break;
                        }
                    }
                    if (c == n_canons) {
                        non_isomorphic.push_back(base);
                        canons.push_back(new graph[m*(size_t)n]);
                        for (c = 0; c < m*(size_t)n; c++) {
                            canons[n_canons][c] = canon[c];
                        }
                        n_canons++;
                    }


                    for (uint cind = 0; cind < cycles.size(); cind++) {
                        int cycle_vertex = base.n() + 3*cind;
                        if (cycles[cind].cycle_sign  == -1) {
                            DELONEEDGE(nauty_graph, cycle_vertex + 1, cycle_vertex + 2, m);
                        }
                    }

                    computing = next_signature_cycles(base, cycles, ST, mapping);
                }

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
                for (auto gpair : colorable) {
                    print_graph(gpair.first, signed_colorable);
                    for (auto c : gpair.second) {
                        signed_colorable << c.first << " " << c.second << " ";
                    }
                    signed_colorable << "\n";
                }
                if (++processed_graphs % PROCESSED_MOD == 0) {
                    cout << "checkpoint: " << processed_graphs << " graphs done\n";
                }
            }
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
    signed_colorable.open(root_dir + vertex_dir + signed_colorable_fn, ofstream::app);
    stats.open(root_dir + vertex_dir + stats_fn, ofstream::app);
    log.open(root_dir + vertex_dir + "generator_log");

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
