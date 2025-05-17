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
int processed_graphs;
int to_skip;


void prepare_transformed_graph(Graph &g, graph* ng, vector<Cycle> &cycles, int m, int n) {
    uint cycle_vertex;
    Cycle cycle;
    // reconstruct base graph
    EMPTYGRAPH(ng,m,n);
    for (auto edge : g.edges) {
        ADDONEEDGE(ng, edge[0], edge[1], m);
    }
    for (size_t i = 0; i < cycles.size(); i++) {
        cycle_vertex = g.n() + 3*i;
        cycle = cycles[i];
        for (size_t j = 0; j < cycle.length(); j++) {
            ADDONEEDGE(ng, cycle_vertex, cycle.vertex_at(j), m);
        }
        // first tail is always connected, second represents the balance
        ADDONEEDGE(ng, cycle_vertex, cycle_vertex + 1, m);
    }
}

void add_tails(Graph &g, graph* ng, vector<Cycle> &cycles, int m) {
    for (uint cind = 0; cind < cycles.size(); cind++) {
        int cycle_vertex = g.n() + 3*cind;
        if (cycles[cind].cycle_sign  == -1) {
            ADDONEEDGE(ng, cycle_vertex, cycle_vertex + 2, m);
        }
    }
}

void remove_tails(Graph &g, graph* ng, vector<Cycle> &cycles, int m) {
    for (uint cind = 0; cind < cycles.size(); cind++) {
        int cycle_vertex = g.n() + 3*cind;
        if (cycles[cind].cycle_sign  == -1) {
            DELONEEDGE(ng, cycle_vertex, cycle_vertex + 2, m);
        }
    }
}

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
        int n_non_isomorphic;
        int max_length;

        DYNALLSTAT(graph, nauty_graph, g_sz);
        DYNALLSTAT(graph, canon, canon_sz);
        DYNALLSTAT(int, lab, lab_sz);
        DYNALLSTAT(int, ptn, ptn_sz);
        DYNALLSTAT(int, orbits, orbits_sz);

        static DEFAULTOPTIONS_GRAPH(options);
        options.getcanon = true;
        options.schreier = true;
        statsblk stats;

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

                int n = base.n() + 3*cycles.size();
                int m = SETWORDSNEEDED(n);
                nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);


                DYNALLOC2(graph,nauty_graph,g_sz,m,n,"malloc");
                DYNALLOC2(graph,canon,canon_sz,m,n,"malloc");
                DYNALLOC1(int,lab,lab_sz,n,"malloc");
                DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
                DYNALLOC1(int,orbits,orbits_sz,n,"malloc");

                prepare_transformed_graph(base, nauty_graph, cycles, m, n);

                computing = true;
                n_canons = 0;
                canons.clear();
                while (computing) {
                    add_tails(base, nauty_graph, cycles, base.m());

                    densenauty(nauty_graph, lab, ptn, orbits, &options, &stats, m, n, canon);

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

                    remove_tails(base, nauty_graph, cycles, base.m());
                    computing = next_signature_cycles(base, cycles, ST, mapping);
                }

            }
            {
                lock_guard<mutex> lock(stream_lock);
                cout << "\n";
                print_graph(base, cout);
                for (auto g : non_isomorphic) {
                    print_graph(g, cout);
                }
                cout.flush();
            }
        }

        // free memory
        DYNFREE(nauty_graph,g_sz);
        DYNFREE(canon,canon_sz);
        DYNFREE(lab, lab_sz);
        DYNFREE(ptn, ptn_sz);
        DYNFREE(orbits,orbits_sz);
        nauty_freedyn();
        nautil_freedyn();
        naugraph_freedyn();
    }
};

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    ofstream log("generator_log");

    int n_threads = thread::hardware_concurrency();
    if (argc > 1) n_threads = stoi(argv[1]);

    vector<SolverThread> solvers(n_threads);
    log << "Starting computation on " << n_threads << " threads.\n";
    vector<thread> threads;
    for (size_t i = 0; i < n_threads; i++) {
        lock_guard<mutex> lock(stream_lock);
        threads.push_back(thread(solvers[i]));
    }
    for (size_t i = 0; i < n_threads; i++) {
        threads[i].join();
        lock_guard<mutex> lock(stream_lock);
        log << "joined a thread\n";
    }
    log << "Done.\n";
}
