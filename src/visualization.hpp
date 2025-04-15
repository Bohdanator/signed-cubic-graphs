#include "graph.hpp"

#include<vector>
#include<string>
#include<sstream>
#include<iostream>
#include<queue>

void graph_to_dot_colors(Graph &graph, ostream &stream, vector<pair<int, int>> colors) {
    stream << "graph unsat_" << graph.n() << "_" << graph.m() << " {\n";
    stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
    for (int i = 0; i < graph.n(); i++)
        stream << "\t" << i << " [shape=circle];\n";
    for (int i = 0; i < graph.m(); i++)
        stream << "\t" << graph.edges[i][0] << " -- " << graph.edges[i][1] << (graph.edges[i][2] < 0 ? " [style=dashed," : "[") << "headlabel=" << colors[i].first << ", taillabel=" << colors[i].second << "]" << ";\n";
    stream << "}";
}

void graph_to_dot(Graph &graph, ostream &stream) {
    stream << "graph unsat_" << graph.n() << "_" << graph.m() << " {\n";
    stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
    for (int i = 0; i < graph.n(); i++)
        stream << "\t" << i << " [shape=circle];\n";
    for (int i = 0; i < graph.m(); i++)
        stream << "\t" << graph.edges[i][0] << " -- " << graph.edges[i][1] << (graph.edges[i][2] < 0 ? " [style=dashed," : "[") << "]" << ";\n";
    stream << "}";
}

void graph_to_dot_ST(Graph &graph, ostream &stream, vector<bool> &ST) {
    stream << "graph unsat_" << graph.n() << "_" << graph.m() << " {\n";
    stream << "\tgraph [size=\"6,6\",ratio=fill];\n";
    for (int i = 0; i < graph.n(); i++)
        stream << "\t" << i << " [shape=circle];\n";
    for (int i = 0; i < graph.m(); i++)
        stream << "\t" << graph.edges[i][0] << " -- " << graph.edges[i][1] << (ST[i] ? "[style=bold, color=red" : (graph.edges[i][2] < 0 ? " [style=dashed," : "[") )<< "]" << ";\n";
    stream << "}";
}

void graph_to_GAP_command(Graph &graph, ostream &stream) {
    stream << "PrintIsoSignatures([], [";
    for (uint i = 0; i < graph.edges.size(); i++) {
        auto e = graph.edges[i];
        stream << "[" << e[0] + 1 << "," << e[1] + 1 << "]";
        if (i < graph.edges.size() - 1) stream << ",";
    }
    stream << "], 1);\n";
}
