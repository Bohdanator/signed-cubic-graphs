KISSAT_FLAGS = -L kissat/build -l kissat
COMPILER_FLAGS = -O3 -Wall -g
SRCS := $(wildcard src/*.cpp)
BINS := $(SRCS:src/%.cpp build/%.o)

.PHONY = all clean

solver: src/solver.cpp src/sat/kissat_wrapper.hpp src/sat/kissat_wrapper.cpp src/graph_utils.hpp src/graph_utils.cpp
	@echo "Compiling solver"
	g++ ${COMPILER_FLAGS} src/solver.cpp src/graph_utils.cpp src/sat/kissat_wrapper.cpp -o build/solver.o ${KISSAT_FLAGS}

generator: src/generator.cpp src/sat/kissat_wrapper.hpp src/sat/kissat_wrapper.cpp src/graph_utils.hpp src/graph_utils.cpp
	@echo "Compiling generator"
	g++ ${COMPILER_FLAGS} src/generator.cpp src/graph_utils.cpp src/sat/kissat_wrapper.cpp -o build/generator.o ${KISSAT_FLAGS}

%: src/%.cpp src/graph_utils.cpp src/sat/kissat_wrapper.cpp
	@echo "Compiling.."
	@echo $<
	g++ ${COMPILER_FLAGS} $< src/graph_utils.cpp src/sat/kissat_wrapper.cpp -o build/$@.o ${KISSAT_FLAGS}

all: ${BINS}

clean: rm dumps/*.o
