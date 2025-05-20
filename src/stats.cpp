#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>
#include "graph.hpp"
#include "graph_utils.hpp"
#include "sat/kissat_wrapper.hpp"

using namespace std;

int main(int argc, char** argv) {
    int total_graphs = 0;
    int total_signatures = 0;
    int total_non_iso = 0;
    int total_snarks = 0;
    int total_strong_colorable = 0;
    Graph least_nonzero_colorable;
    int lnzc = 10000000;
    Graph most_nonstrong_colorable;
    int mnsc = 0;
    Graph most_snark;
    float ms = 0;
    int msint = 0;
    int msnoniso = 0;
    Graph most_colorable;
    float mc = 1;
    int mcint = 0;
    int mcnoniso = 0;
    vector<int> n_snarks_vec;
    vector<int> n_iso_vec;
    Graph least_iso;
    int lizo = 10000000;

    Graph buf;
    int noniso, snark;
    string word;
    cout << "Parsing graphs...";
    while(parse_graph(buf, cin)) {
        total_graphs++;
        total_signatures += n_signatures(buf);
        cin >> word >> noniso;
        cin >> word >> snark;

        n_iso_vec.push_back(noniso);
        n_snarks_vec.push_back(snark);
        total_non_iso += noniso;
        total_snarks += snark;

        if (noniso < lizo) {
            least_iso = buf;
            lizo = noniso;
        }
        if (snark == 0) {
            total_strong_colorable += 1;
        }
        if (snark < noniso && snark > mnsc) {
            mnsc = snark;
            most_nonstrong_colorable = buf;
        }
        if (snark > 0 && snark < lnzc) {
            lnzc = snark;
            least_nonzero_colorable = buf;
        }
        if (snark < noniso && ((float)snark / (float)noniso) > ms) {
            ms = snark / noniso;
            msint = snark;
            msnoniso = noniso;
            most_snark = buf;
        }
        if (snark > 0 && ((float)snark / (float)noniso) < mc) {
            mc = snark / noniso;
            mcint = snark;
            mcnoniso = noniso;
            most_colorable = buf;
        }
    }

    sort(n_iso_vec.begin(), n_iso_vec.end());
    sort(n_snarks_vec.begin(), n_snarks_vec.end());

    cout << "done\nSTATS:\n";
    cout << "Total graphs: " << total_graphs << endl;
    cout << "Total non equivalent signatures: " << total_signatures << " avg " << total_signatures / total_graphs << endl;
    cout << "Total non switching isomorphic: " << total_non_iso << ", " << total_non_iso * 100.0 / total_signatures << " %, avg " << total_non_iso / total_graphs << endl;
    cout << "Total signed snarks " << total_snarks << ", " <<  100.0 * total_snarks / total_non_iso << " %" << "\n";
    cout << "Total strong colorable " << total_strong_colorable << ", " << 100.0 * total_strong_colorable / total_non_iso << "%\n\n";

    cout << "Swithing iso distribution\n";
    cout << n_iso_vec[0] << " " << n_iso_vec[n_iso_vec.size()/2] << " " << n_iso_vec[n_iso_vec.size() - 1] << "\n\n";
    cout << "Snarks distribution\n";
    cout << n_snarks_vec[0] << " " << n_snarks_vec[n_snarks_vec.size()/2] << " " << n_snarks_vec[n_snarks_vec.size() - 1] << "\n\n";

    cout << "Least non iso overall " << lizo << "\n";
    print_graph(least_iso, cout);
    cout << "Least colorable overall, " << lnzc << "\n";
    print_graph(least_nonzero_colorable, cout);
    cout << "Least colorable percentage, " << ms  << " " << msint << " " << msnoniso << endl;
    print_graph(most_snark, cout);
    cout << "Most colorable overall, " << mnsc << "\n";
    print_graph(most_nonstrong_colorable, cout);
    cout << "Most colorable percentage, " << (float)1 - mc << " " << mcint << " " << mcnoniso << endl;
    print_graph(most_colorable, cout);

    cout << endl << "Output for latex table\n";
    cout << least_iso.n() << " & " << n_iso_vec.size() << " & "  << total_signatures / total_graphs << " & " << total_signatures << " & " << total_non_iso << " & " << n_iso_vec[0] << " & " << (float)total_non_iso / (float)total_graphs << " & " << total_non_iso * 100.0 / total_signatures << "\\% \\\\\n";
    cout << endl;
    cout << least_iso.n() << " & " << n_iso_vec.size() << " & "  << total_non_iso << " & " << total_strong_colorable << " & " << 100.0 * total_strong_colorable / total_graphs << "\\% & " << total_snarks << " & " << 100.0 * total_snarks / total_non_iso << "\\% \\\\\n";
    cout << "Done.\n";
}
