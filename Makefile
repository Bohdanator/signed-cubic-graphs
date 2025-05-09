KISSAT_FLAGS = -L kissat/build -l kissat
NAUTY_FLAGS = -L nauty -l:nautyT.a
COMPILER_FLAGS = -O4 -Wall -g
SRCS := $(wildcard src/*.cpp)
BINS := $(SRCS:src/%.cpp build/%.o)

.PHONY = all clean

%: src/%.cpp src/sat/kissat_wrapper.cpp
	@echo "Compiling..."
	@echo $<
	g++ ${COMPILER_FLAGS} $< src/sat/kissat_wrapper.cpp -o build/$@ ${KISSAT_FLAGS} ${NAUTY_FLAGS}

all: ${BINS}

clean: rm build/*.o
