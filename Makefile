CXX=g++
CXXFLAGS=-Wall

BINARIES = cosetsolver
TESTBINARIES = cubepos_test

all: $(BINARIES) $(TESTBINARIES)

test: $(TESTBINARIES)
	./cubepos_test

cosetsolver:
	$(CXX) $(CXXFLAGS) -o cosetsolver cosetsolver.cpp coset.cpp cubepos.cpp subcube.cpp

clean:
	rm $(BINARIES) $(TESTBINARIES)

cubepos_test:
	$(CXX) $(CXXFLAGS) -o cubepos_test cubepos_test.cpp cubepos.cpp