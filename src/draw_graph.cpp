#include<bits/stdc++.h>
#include "graph.h"

int main() {
    Graph graph;
    graph.parse_from_stream(cin);
    graph.graph_to_dot(cout);
}
