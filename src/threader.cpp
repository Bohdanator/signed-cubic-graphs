#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<thread>
#include "graph.hpp"
#include "graph_utils.hpp"
#include "sat/kissat_wrapper.hpp"
#include "visualization.hpp"

using namespace std;

vector<vector<Graph>> snarks;
vector<int> number_of_cycles;
vector<bool> cycles_not_enough;

class SolverThread
{
public:
    Graph graph;
    int id;

    SolverThread() = delete;
    SolverThread(Graph g, int i) : graph(g), id(i) {}

    void operator() () {
        vector<Cycle> cycles;
        int max_length;
        vector<uint> cycle_space = generate_cycles_with_coverage(graph, cycles, max_length);
        uint sum = 0;
        for (auto x : cycle_space) {
            sum |= x;
        }
        if (!cycles_not_enough[id] && sum < (1 << (graph.m())) - 1) {
            cout << "cycle space doesn't generate whole space\n" << sum << " instead of " << (1 << (graph.m())) - 1 << endl;
            //print_graph(graph,cout);
            cycles_not_enough[id] = true;
        }
        number_of_cycles[id] = cycles.size();
        cout << cycles.size() << " cycles total. " << max_length << " max length\n";
    }
};

int main(int argc, char** argv) {
    // if (argc < 2) {
    //     cout << "Usage: generator.o thread(default=1) skip(default=0)\n";
    //     return 0;
    // }

    // INIT
    int threads_n = (argc <= 1 ? 1 : stoi(argv[1]));
    int skip_n = (argc <= 2 ? 0 : stoi(argv[2]));

    string log_fn = "snark_generator_log";
    ofstream log(log_fn);

    // SKIP
    Graph tmp;
    for (int i = 0; i < skip_n; i++) {
        graph_from_edge_list(tmp, cin);
    }

    // COMPUTE
    int graphs_total = 0;
    int cycles_total = 0;
    int graphs_without_basis = 0;
    int max_cycles = 0;
    int min_cycles = INT32_MAX;
    int solvers_n = threads_n;
    Graph buffer;
    while(solvers_n == threads_n) {
        // initiate solvers
        solvers_n = 0;
        vector<SolverThread> solvers;
        while(solvers_n < threads_n && parse_graph(buffer, cin)) {
            solvers.push_back(SolverThread(buffer, solvers_n));
            solvers_n++;
            buffer.clear();
        }
        // start threads
        vector<thread> threads;
        number_of_cycles.resize(solvers_n);
        cycles_not_enough.resize(solvers_n);
        for (uint i = 0; i < solvers.size(); i++) {
            threads.push_back(thread(solvers[i]));
        }
        // join
        for (uint i = 0; i < threads.size(); i++) {
            threads[i].join();
            graphs_total++;
            cycles_total += number_of_cycles[i];
            if (graphs_total % 100 == 0) {
                log << graphs_total << " graphs done, "
                    << cycles_total << " cycles\n";
                log.flush();
            }
            if (cycles_not_enough[i]) {
                graphs_without_basis++;
            }
            max_cycles = max(max_cycles, number_of_cycles[i]);
            min_cycles = min(min_cycles, number_of_cycles[i]);
        }
    }
    log << graphs_total << " graphs overall done." << endl;
    log << graphs_without_basis << " graphs can't be covered with cycles\n";
    log << cycles_total << " cycles.\n" << cycles_total / graphs_total << "\n";
    log << "maximum " << max_cycles << "; minimum" << min_cycles;
}
