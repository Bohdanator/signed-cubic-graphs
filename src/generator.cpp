#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<thread>
#include<mutex>
#include "sat/kissat_wrapper.hpp"
#include "visualization.hpp"
#include "isomorphism_utils.hpp"

using namespace std;

mutex read_lock;
mutex write_lock;

class SolverThread
{
public:
    Graph base;
    vector<Graph> non_isomorphic;
    int n_non_isomorphic;

    SolverThread() = default;

    void operator() () {
        while (1) {
            non_isomorphic.clear();
            read_lock.lock();
            if (!load_graph()) break;
            read_lock.unlock();
            n_non_isomorphic = generate_non_isomorphic_graphs(base, non_isomorphic);
            write_lock.lock();
            print_results();
            write_lock.unlock();
        }
    }

    bool load_graph() {
        cout << "locked read " << this_thread::get_id() << "\n";
        base.clear();
        return parse_graph(base, cin);
    }

    void print_results() {
        cout << "locked write " << this_thread::get_id() << "\n";
        cout << "\n" << n_non_isomorphic << "\n";
        for (auto g : non_isomorphic) {
            print_graph(g, cout);
        }
    }
};

int main(int argc, char** argv) {
    int n_threads = 1;
    if (argc > 1) n_threads = stoi(argv[1]);

    vector<SolverThread> solvers(n_threads);
    cout << "Starting computation with " << n_threads << " threads.\n";
    vector<thread> threads;
    for (size_t i = 0; i < n_threads; i++) {
        threads.push_back(thread(solvers[i]));
    }
    for (size_t i = 0; i < n_threads; i++) {
        threads[i].join();
    }
    cout << "Done.\n";
}
