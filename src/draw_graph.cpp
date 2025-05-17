#include<bits/stdc++.h>
#include "graph.hpp"
#include "graph_utils.hpp"
#include "visualization.hpp"

int main() {
    Graph graph;
    parse_graph(graph, cin);
    graph_to_dot(graph, cout);
}
