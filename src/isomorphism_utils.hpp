#include<vector>
#include<string>
#include<sstream>
#include<iostream>
#include<queue>

#include "graph.hpp"
#include "graph_utils.hpp"

#define MAXN 1000
#include "nauty/nauty.h"
#include "nauty/gtools.h"

int generate_non_isomorphic_graphs(Graph &base, vector<Graph> &non_isomorphic) {
    vector<bool> ST;
    spanning_tree(base, ST);
    vector<Cycle> cycles;
    int max_length;
    generate_cycles_with_coverage(base, cycles, max_length);
    vector<vector<int>> mapping;
    edge_cycle_mapping(cycles, mapping, base.m());

    static DEFAULTOPTIONS_GRAPH(options);
    options.getcanon = true;
    options.schreier = true;
    statsblk stats;
    int n = base.n() + 3*cycles.size();
    int m = SETWORDSNEEDED(n);
    nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);

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

    vector<graph*> canons;
    size_t n_canons = 0;

    bool computing = true;
    size_t k, c;
    k = 0;
    while (computing) {
        for (uint cind = 0; cind < cycles.size(); cind++) {
            int cycle_vertex = base.n() + 3*cind;
            if (cycles[cind].cycle_sign  == -1) {
                ADDONEEDGE(nauty_graph, cycle_vertex, cycle_vertex + 2, m);
            }
        }
        // compute canonical form
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
        for (uint cind = 0; cind < cycles.size(); cind++) {
            int cycle_vertex = base.n() + 3*cind;
            if (cycles[cind].cycle_sign  == -1) {
                DELONEEDGE(nauty_graph, cycle_vertex, cycle_vertex + 2, m);
            }
        }
        computing = next_signature_cycles(base, cycles, ST, mapping);
    }

    return n_canons;
}
