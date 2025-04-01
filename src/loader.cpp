#include<bits/stdc++.h>
#include "graph.hpp"
#include "graph_utils.hpp"
#include "visualization.hpp"

using namespace std;

void load_graph(istream& fin, Graph &graph) {
    string buf;
    graph.clear();

    getline(fin, buf); // Graph x, blablabla, we don't need this
    getline(fin, buf);
    int n = buf.size();
    graph.init(n);
    for (int r = 0; r < n; r++) {
        if (r > 0) getline(fin, buf);
        for (int c = 0; c < buf.size(); c++) {
            if (buf[c] == '1' && r <= c) {
                graph.add_edge(r,c,1);
            }
        }
    }
}

int main(int argc, char** argv) {
    Graph graph;
    if (argc > 1) {
        ifstream in(argv[1]);
        string buf;
        while(getline(in, buf)) {
            load_graph(in, graph);
            print_graph(graph, cout);
        }
    } else {
        while(!cin.eof()) {
            load_graph(cin, graph);
            print_graph(graph, cout);
        }
    }
}
