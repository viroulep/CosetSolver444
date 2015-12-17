#include "cubepos.h"
#include <iostream>
#include <cstring>

void rotation_order(){
	std::cout << "cubepos_test: all rotations have an order of 4... ";

	cubepos cp;
	char testok = 1;
	for (int m=0; m<N_MOVES; m++){
		cp.identity();
		cp.move(m);
		cp.move(m);
		cp.move(m);
		cp.move(m);
		if (cp != identity_cube){
			testok = 0;
			break;
		}
	}
	if (testok)
		std::cout << "passed" << std::endl;
	else
		std::cout << "failed" << std::endl;
}

void symmetry_order(){
	std::cout << "cubepos_test: all symmetries have the correct order... ";

	int testbad = 0;
	int symmetry_orders[48] = {
		1, 2, 2, 2, 2, 2, 2, 2, 
		4, 4, 2, 2, 2, 2, 4, 4, 
		3, 3, 3, 3, 6, 6, 6, 6, 
		2, 4, 2, 4, 4, 2, 4, 2, 
		3, 3, 3, 3, 6, 6, 6, 6, 
		4, 2, 2, 4, 4, 2, 2, 4
	};

	unsigned char current_edges[24];
	unsigned char current_centers[24];
	unsigned char solved_edges[24];
	unsigned char solved_centers[24];

	for (int i=0; i<24; i++){
		solved_edges[i] = i;
		solved_centers[i] = i;
	}

	for (int sym=0; sym<48; sym++){
		int symorder = symmetry_orders[sym];
		std::memcpy(current_edges, solved_edges, 24);
		std::memcpy(current_centers, solved_centers, 24);

		for (int o=0; o<symorder; o++){
			for (int i=0; i<24; i++){
				current_edges[i] = cubepos::symEdges[sym][current_edges[i]];
				current_centers[i] = cubepos::symCenters[sym][current_centers[i]];
			}
			if ((o+1) < symorder){ // Not the last time
				/* Check if we return to the solved state earlier than expected */
				if ((std::memcmp(current_edges, solved_edges, 24) == 0) || (std::memcmp(current_centers, solved_centers, 24) == 0)) {
					testbad = sym;
				}
			}
		}
		if ((std::memcmp(current_edges, solved_edges, 24) != 0) || (std::memcmp(current_centers, solved_centers, 24) != 0)) {
			testbad = sym;
		}
	}

	if (testbad != 0)
		std::cout << "failed (symmetry " << testbad << ")" << std::endl;
	else
		std::cout << "passed" << std::endl;
}

void symmetry_inverse(){
	std::cout << "cubepos_test: all symmetries have the right inverse... ";

	int testok = 1;
	for (int sym=0; sym<48; sym++){
		for (int i=0; i<24; i++){
			if (cubepos::symEdges[sym][cubepos::symEdges[cubepos::invSymIdx[sym]][i]] != i)
				testok = 0;
			if (cubepos::symCenters[sym][cubepos::symCenters[cubepos::invSymIdx[sym]][i]] != i)
				testok = 0;
		}
	}

	if (testok)
		std::cout << "passed" << std::endl;
	else
		std::cout << "failed" << std::endl;
}

int main() {

	cubepos cp;
	cp.init();

	rotation_order();
	symmetry_order();
	symmetry_inverse();

	return 0;
}