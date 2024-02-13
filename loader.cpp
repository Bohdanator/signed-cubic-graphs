#include<bits/stdc++.h>
#include "graph.h"

using namespace std;

void load_graph(istream& fin, Graph &graph) {
    string buf;

    getline(fin, buf); // Graph x, blablabla, we don't need this
    getline(fin, buf);
    int n = buf.size();
    graph = Graph(n);
    for (int r = 0; r < n; r++) {
        if (r > 0) getline(fin, buf);
        for (int c = 0; c < buf.size(); c++) {
            if (buf[c] == '1' && r <= c) {
                graph.add_edge(r, c);
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
            graph.print();
        }
    } else {
        while(!cin.eof()) {
            load_graph(cin, graph);
            graph.print();
        }
    }
}
