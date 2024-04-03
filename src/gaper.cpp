#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<thread>
#include "graph.hpp"
#include "graph_utils.hpp"
#include "sat/kissat_wrapper.hpp"

using namespace std;

int main() {
    Graph graph;
    while (graph_from_edge_list(graph, cin)) {
        graph_to_GAP_command(graph, cout);
    }
    cout << "quit;";
}
