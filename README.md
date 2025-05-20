# Edge coloring signed cubic graphs

This repository provides tools for research of edge coloring cubic graphs.

## Build

Generation requires `kissat` and `nauty` with thread-safe environment built.

Building any program in `src/` directory is possible with `make <filename>` which compiles `<filename>.cpp` into `build/<filename>` executable. Compiling all binaries is possible with `make all`.

## Code structure

* `graph.hpp`, `graph_utils.hpp`, `visualization.hpp`, `isomorphism_utils.hpp` are helper files defining the graph struct and containing functions for generating signed graphs, filtering them for switching isomorphisms and finding edge colorings. Note: there is an unresolved issue with memory leakage when using `isomorphism_utils.hpp`.
* `color_solver.cpp` is the main component combining switching isomorphisms and edge coloring. It is a multi-threaded program that reads graphs in WYSIWYG format from stdin and produces multiple files with data about the signed graphs. The first argument is the directory where the data will be written, second optional argument is the number of threads and third optional argument is the number of graphs to skip in case the computation was interrupted.
* `cleaner.cpp` is a tool for refining potentially inconsistent data generated with `color_solver`. Because of threading when the computation is interrupted some graphs are processed twice and some might get skipped. This program reads graphs from stdin and compares them against the `stats` file. Missing graphs are processed and added and graphs processed multiple times are reduced to just one datapoint.
* `stats.cpp` reads some `stats.txt` file generated with `color_solver` from stdin and produces an overview of the data

Other files are not up to date or deprecated.
