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

    graph nauty_graph[MAXN*MAXM];
    int n = base.n() + 3*cycles.size();
    int m = SETWORDSNEEDED(n);
    nauty_check(WORDSIZE, m, n, NAUTYVERSIONID);
    EMPTYGRAPH(nauty_graph, m, n);

    uint cycle_vertex;
    Cycle cycle;
    // reconstruct base graph
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
    canons.push_back(new graph[MAXM*MAXN]);
    size_t n_canons = 0;
    static DEFAULTOPTIONS_DENSEGRAPH(options);
    options.getcanon = true;
    options.schreier = true;

    bool computing = true;
    size_t k, c;
    int lab[MAXN], ptn[MAXN], orbits[MAXN];
    statsblk stats;
    k = 0;
    while (computing) {
        for (uint cind = 0; cind < cycles.size(); cind++) {
            int cycle_vertex = base.n() + 3*cind;
            if (cycles[cind].cycle_sign  == -1) {
                ADDONEEDGE(nauty_graph, cycle_vertex, cycle_vertex + 2, m);
            }
        }

        // compute canonical form
        densenauty(nauty_graph, lab, ptn, orbits, &options, &stats, m, n, canons[n_canons]);

        for (c = 0; c < n_canons; c++) {
            for (k = 0; k < m*(size_t)n; k++) {
                if (canons[c][k] != canons[n_canons][k]) {
                    break;
                }
            }
            if (k >= m*(size_t)n) {
                // isomorphic with a previous graph
                break;
            }
        }
        if (c == n_canons) {
            n_canons++;
            canons.push_back(new graph[MAXM*MAXN]);
            non_isomorphic.push_back(base);
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
