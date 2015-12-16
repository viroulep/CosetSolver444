CXX=g++
CXXFLAGS=-Wall

BINARIES = cosetsolver

all: $(BINARIES)

cosetsolver:
	$(CXX) $(CXXFLAGS) -o cosetsolver cosetsolver.cpp coset.cpp cubepos.cpp subcube.cpp
