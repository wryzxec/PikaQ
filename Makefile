CXX = g++
CXXFLAGS = -std=c++20 -O0 -march=native -Iinclude \
           -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -pthread
GTEST_LIBS = -lgtest -lgtest_main

all: spsc_tests benchmarks

spsc_tests: tests/spsc_tests.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(GTEST_LIBS) $< -o $@

benchmarks: src/benchmarks.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lbenchmark $< -o $@

clean:
	rm -f spsc_tests benchmarks

