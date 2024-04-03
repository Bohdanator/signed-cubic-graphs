#ifndef GRAPH_UTILS_HPP
#define GRAPH_UTILS_HPP

#include<iostream>

int parse_graph(Graph &graph, istream &stream);
void print_graph(Graph &graph, ostream &out);
void print_graph_adj(Graph &graph, ostream &out);
int graph_from_edge_list(Graph &graph, istream &stream);
void graph_from_signed_adj_matrix(Graph &graph, vector<vector<int>> &matrix);
void graph_to_signed_adj_matrix(Graph &graph, vector<vector<int>> &matrix);
void graph_to_GAP_command(Graph &graph, ostream &stream);

void graph_to_dot_colors(Graph &graph, ostream &stream, vector<pair<int, int>> colors);
void graph_to_dot(Graph &graph, ostream &stream);
void graph_to_dot_ST(Graph &graph, ostream &stream, vector<bool> &ST);
void graph_to_SAT(Graph &graph, vector<vector<int>> &sat);

int number_of_triangles(Graph& graph);
void spanning_tree(Graph& graph, vector<bool>& ST);
void next_signature(Graph &graph, vector<bool>& ST);
void sat_result_to_coloring(std::vector<int> &sol, std::vector<pair<int, int>> &coloring);
bool same_base_graph(Graph &g1, Graph &g2);

#endif
