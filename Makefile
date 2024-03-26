KISSAT_FLAGS = -L kissat/build -l kissat
COMPILER_FLAGS = -O3 -Wall -g
.PHONY = all clean

solver: src/solver.cpp src/sat/kissat_wrapper.hpp src/sat/kissat_wrapper.cpp
	@echo "Compiling solver"
	g++ ${COMPILER_FLAGS} src/solver.cpp src/sat/kissat_wrapper.cpp -o build/solver.o ${KISSAT_FLAGS}

all: solver

clean: rm dumps/*.o
