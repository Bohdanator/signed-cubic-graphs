#include<iostream>
#include<fstream>
#include<syncstream>
#include<vector>
#include<string>
#include<thread>
#include<mutex>
#include "sat/kissat_wrapper.hpp"
#include "visualization.hpp"
#include "isomorphism_utils.hpp"

using namespace std;

mutex stream_lock;
mutex nauty_lock;

class SolverThread
{
public:
    Graph base;
    vector<Graph> non_isomorphic;
    int n_non_isomorphic;

    SolverThread() = default;

    void operator() () {
        while (1) {
            {
                lock_guard<mutex> lock(stream_lock);
                if (!load_graph()) break;
            }
            {
                non_isomorphic.clear();
                n_non_isomorphic = generate_non_isomorphic_graphs(base, non_isomorphic);
            }
            {
                lock_guard<mutex> lock(stream_lock);
                print_results();
            }
        }
    }

    bool load_graph() {
        base.clear();
        return parse_graph(base, cin);
    }

    void print_results() {
        cout << "\n";
        print_graph(base, cout);
        for (auto g : non_isomorphic) {
            print_graph(g, cout);
        }
    }
};

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    int n_threads = thread::hardware_concurrency();
    if (argc > 1) n_threads = stoi(argv[1]);

    vector<SolverThread> solvers(n_threads);
    cout << "Starting computation on " << n_threads << " threads.\n";
    vector<thread> threads;
    for (size_t i = 0; i < n_threads; i++) {
        lock_guard<mutex> lock(stream_lock);
        threads.push_back(thread(solvers[i]));
    }
    for (size_t i = 0; i < n_threads; i++) {
        threads[i].join();
        lock_guard<mutex> lock(stream_lock);
        cout << "joined a thread\n";
    }
    cout << "Done.\n";
}
