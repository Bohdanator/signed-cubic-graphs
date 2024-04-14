#include "graph.hpp"
#include "graph_utils.hpp"
#include<vector>
#include<iostream>

using namespace std;

int main() {
    Graph graph;
    while(parse_graph(graph, cin)) {
        graph_to_unsigned_repr(graph);
        print_graph_adj(graph, cout);
    }
}
