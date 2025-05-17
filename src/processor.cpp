#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include "graph.hpp"
#include "graph_utils.hpp"

using namespace std;

int main(int argc, char** argv) {
    string fname = "stats.txt";
    if (argc > 1) fname = argv[1];

    ifstream stats(fname);
    Graph buffer;
    int non_iso, col, snark;
    while (parse_graph(buffer, stats)) {
        stats >> non_iso >> col >> snark;
        if (snark == 1) {
            cout << "only one snark\n";
            print_graph(buffer, cout);
        }
        if (col == 1) {
            cout << "only one colorable\n";
            print_graph(buffer, cout);
        }
    }
}
