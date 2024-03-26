#include<iostream>
#include<fstream>
#include<vector>
#include<string>

#include "graph.hpp"
#include "graph_utils.hpp"

using namespace std;

int main(int argc, char** argv) {
    string fn = argv[1];
    ifstream in(fn);
    Graph curr_graph, prev_graph;
    prev_graph.init(18);
    int signatures = 0;
    int max_signatures = 0;
    int total = 0;

    while(parse_graph(curr_graph, in)) {
        total++;
        if (same_base_graph(curr_graph, prev_graph)) {
            signatures++;
            continue;
        }
        if (signatures >= max_signatures) {
            prev_graph.print();
        }
        signatures = 0;
        max_signatures = (1 << (curr_graph.m() - curr_graph.n() + 1)) - 1;
        prev_graph = curr_graph;
    }
    if (signatures >= max_signatures) {
        prev_graph.print();
    }
}