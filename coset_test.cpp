#include <iostream>
#include <cstdlib>
#include <ctime>
#include "coset.h"

void pack_unpack(){
	std::cout << "coset_test: unpacking and packing gives the same coordinate... ";

	cubepos cp;
	coset c;

	int testok = 1;
	for (unsigned int coord=0; coord<COSET_N_RAW_COORD; coord++) {
		c.unpack(cp, coord);
		unsigned int new_coord = c.pack(cp);
		if (new_coord != coord) {
			testok = 0;
			break;
		}
	}

	if (testok)
		std::cout << "passed" << std::endl;
	else
		std::cout << "failed" << std::endl;
}

void move_cp(){
	std::cout << "coset_test: moving on coset or cubepos gives the same state... ";

	cubepos cp;
	coset c;
	coset c2;

	int testok = 1;
	for (int i=0; i<1000; i++){
		cp.identity();
		c.pack_all(cp);
		for (int m=0; m<100; m++){
			int move = (int) ( N_MOVES * drand48() );
			cp.move(move);
			c.moveTo(cubepos::moves2stage[move], c);
		}
		c2.pack_all(cp);
		if (c.center_rl_sym != c2.center_rl_sym) {
			testok = 0;
			break;
		}
	}

	if (testok)
		std::cout << "passed" << std::endl;
	else
		std::cout << "failed" << std::endl;
}

int main() {

	coset c;
	c.init();

	if (lrand48() == 0)
		srand48(time(0));

	pack_unpack();
	move_cp();

	return 0;
}
