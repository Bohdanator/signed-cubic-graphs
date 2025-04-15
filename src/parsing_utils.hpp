#include "graph.hpp"

#include<string>
#include<sstream>
#include<iostream>

#include "nauty/gtools.h"

using namespace std;

void load_base_graph(ostream &out, Graph &graph);
void load_signature();
void write_base_graph();
void write_signature();
