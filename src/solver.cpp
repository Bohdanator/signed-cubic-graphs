#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<thread>
#include "graph.hpp"
#include "graph_utils.hpp"
#include "sat/kissat_wrapper.hpp"

using namespace std;

vector<vector<Graph>> snarks;

class SolverThread
{
public:
    Graph graph;
    int id;

    SolverThread() = delete;
    SolverThread(Graph g, int i) : graph(g), id(i) {}

    void operator() () {
        int signatures_n = (1 << (graph.m() - graph.n() + 1));
        vector<bool> ST;
        spanning_tree(graph, ST);

        for (int i = 0; i < signatures_n; i++) {
            vector<vector<int>> sat_instance;
            vector<int> sol;
            KissatWrapper sat_solver;
            graph_to_SAT(graph, sat_instance);
            int result = sat_solver.solve(sat_instance, sol);
            if (result != 10) {
                // UNSAT
                snarks[id].push_back(graph);
            }
            next_signature(graph, ST);
        }
    }
};

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: generator.o <snarks_file> <strong_colorables_file> thread(default=1) skip(default=0)\n";
        return 0;
    }

    // INIT
    string snarks_fn = argv[1];
    string colorable_fn = argv[2];
    int threads_n = (argc <= 3 ? 1 : stoi(argv[3]));
    int skip_n = (argc <= 4 ? 0 : stoi(argv[4]));

    ofstream snarks_out(snarks_fn, ofstream::ate);
    ofstream colorable_out(colorable_fn, ofstream::ate);

    // SKIP
    Graph tmp;
    for (int i = 0; i < skip_n; i++) {
        graph_from_edge_list(tmp, cin);
    }

    // COMPUTE
    int base_graphs_done = 0;
    int snarks_n = 0;
    int solvers_n = threads_n;
    Graph buffer;
    while(solvers_n == threads_n) {
        // initiate solvers
        solvers_n = 0;
        vector<SolverThread> solvers;
        while(graph_from_edge_list(buffer, cin) && solvers_n < threads_n) {
            solvers.push_back(SolverThread(buffer, solvers_n));
            solvers_n++;
        }
        // start threads
        vector<thread> threads;
        snarks.clear();
        snarks.resize(solvers_n);
        for (uint i = 0; i < solvers.size(); i++) {
            threads.push_back(thread(solvers[i]));
        }
        // join
        for (uint i = 0; i < threads.size(); i++) {
            threads[i].join();
            base_graphs_done++;
            if (base_graphs_done % 100 == 0) {
                cout << base_graphs_done << " graphs done, "
                     << snarks_n << "snarks\n";
            }
            if (snarks[i].size() == 0) {
                solvers[i].graph.print(colorable_out);
                continue;
            }
            snarks_n += snarks[i].size();
            for (auto &graph : snarks[i]) {
                graph.print(snarks_out);
            }
        }
        snarks_out.flush();
        colorable_out.flush();
    }
    cout << base_graphs_done << " graphs overall done." << endl;
    snarks_out.close();
    colorable_out.close();
}
