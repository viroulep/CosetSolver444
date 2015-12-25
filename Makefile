CXX=g++
CXXFLAGS=-W -Wall -std=c++11
LDFLAGS=

BINARIES = cosetsolver
TESTBINARIES = cubepos_test coset_test subcube_test

all: $(BINARIES) $(TESTBINARIES)

test: $(TESTBINARIES)
	./cubepos_test && ./coset_test && ./subcube_test

%.o: %.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

cosetsolver: cosetsolver.o cubepos.o coset.o subcube.o
	$(CXX) -o $@ $^ $(LDFLAGS)

cubepos_test: cubepos_test.o cubepos.o
	$(CXX) -o $@ $^ $(LDFLAGS)

coset_test: coset_test.o coset.o cubepos.o
	$(CXX) -o $@ $^ $(LDFLAGS)

subcube_test: subcube_test.o cubepos.o subcube.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm *.o

mrproper:
	rm $(BINARIES) $(TESTBINARIES)

