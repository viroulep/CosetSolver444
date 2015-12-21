CXX=g++
CXXFLAGS=-Wall

BINARIES = cosetsolver
TESTBINARIES = cubepos_test coset_test subcube_test

all: $(BINARIES) $(TESTBINARIES)

test: $(TESTBINARIES)
	./cubepos_test && ./coset_test && ./subcube_test

cosetsolver:
	$(CXX) $(CXXFLAGS) -o cosetsolver cosetsolver.cpp coset.cpp cubepos.cpp subcube.cpp

clean:
	rm $(BINARIES) $(TESTBINARIES)

cubepos_test:
	$(CXX) $(CXXFLAGS) -o cubepos_test cubepos_test.cpp cubepos.cpp

coset_test:
	$(CXX) $(CXXFLAGS) -o coset_test coset_test.cpp cubepos.cpp coset.cpp

subcube_test:
	$(CXX) $(CXXFLAGS) -o subcube_test subcube_test.cpp subcube.cpp cubepos.cpp coset.cpp

