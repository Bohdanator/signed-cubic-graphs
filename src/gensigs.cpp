#include "graph.hpp"
#include "graph_utils.hpp"
#include<vector>
#include<iostream>

using namespace std;

int main() {
    Graph graph;
    while(graph_from_edge_list(graph, cin)) {
        vector<bool> ST;
        spanning_tree(graph, ST);
        for (int i = 0; i < n_signatures(graph); i++) {
            print_graph(graph, cout);
            next_signature(graph, ST);
        }
    }
}
