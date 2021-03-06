#include <iostream>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include "subcube.h"

void pack_unpack(){
	std::cout << "subcube_test: unpacking and packing gives the same coordinate... ";

	cubepos cp;
	subcube sc;

	int testok = 1;
	for (unsigned short cr=0; cr<SUBCUBE_N_COORD_CENTER_R; cr++) {
		sc.unpack_center_r(cp, cr);
		unsigned short new_cr = sc.pack_center_r(cp);
		if (new_cr != cr) {
			testok = 0;
			break;
		}
	}

	for (unsigned int cfb=0; cfb<SUBCUBE_N_COORD_CENTER_FB; cfb++) {
		sc.unpack_center_fb(cp, cfb);
		unsigned int new_cfb = sc.pack_center_fb(cp);
		if (new_cfb != cfb) {
			testok = 0;
			break;
		}
	}

	for (unsigned int e=0; e<SUBCUBE_N_COORD_EDGES; e++) {
		sc.unpack_edge(cp, e);
		unsigned int new_e = sc.pack_edge(cp);
		if (new_e != e) {
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
	std::cout << "subcube_test: moving on subcube or cubepos gives the same state... ";

	cubepos cp;
	subcube sc;
	subcube sc2;

	int testok = 1;
	for (int i=0; i<1000; i++){
		cp.identity();
		for (int m=0; m<100; m++){
			int move = (int) ( SUBCUBE_N_MOVES_ALL * drand48() );
			cp.move(cubepos::stage2moves[move]);
		}
		sc.pack_all(cp);
		for (int m=0; m<100; m++){
			int move = (int) ( SUBCUBE_N_MOVES_ALL * drand48() );
			cp.move(cubepos::stage2moves[move]);
			sc.moveTo(move, sc);
		}
		sc2.pack_all(cp);
		sc.canonize();
		sc2.canonize();
		if ((sc.edge_sym != sc2.edge_sym) || (sc.center_r != sc2.center_r) || (sc.center_fb != sc2.center_fb)) {
			testok = 0;
			break;
		}
	}

	if (testok)
		std::cout << "passed" << std::endl;
	else
		std::cout << "failed" << std::endl;
}

void move_cp_sc(){
	std::cout << "subcube_test: moving on subcube inside H or cubepos inside H gives the same state... ";

	cubepos cp;
	cubepos cp2;
	subcube sc;
	subcube sc2;

	int testok = 1;
	for (int i=0; i<1000; i++){
		cp.identity();
		for (int m=0; m<100; m++){
			int move = (int) ( SUBCUBE_N_MOVES * drand48() );
			cp.move(cubepos::stage2moves[move]);
		}
		sc.pack_all(cp);
		bool isSC = sc.convertToSC();
		if (!isSC) {
			testok = -1;
			break;
		}
		for (int m=0; m<100; m++){
			int move = (int) ( SUBCUBE_N_MOVES * drand48() );
			cp.move(cubepos::stage2moves[move]);
			sc.moveToSC(move, sc);
		}
		cp.rightMult(16*(int)(3*drand48()), cp2); // Random orientation
		sc2.pack_all(cp2);
		bool isSC2 = sc2.convertToSC();
		if (!isSC2) {
			testok = -1;
			break;
		}
		sc.canonizeSC();
		sc2.canonizeSC();
		if ((sc.edge_sym != sc2.edge_sym) || (sc.center_r != sc2.center_r) || (sc.center_fb != sc2.center_fb)) {
			testok = 0;
			break;
		}
	}

	if (testok > 0)
		std::cout << "passed" << std::endl;
	else if (testok == 0)
		std::cout << "failed (different state)" << std::endl;
	else
		std::cout << "failed (not to SC)" << std::endl;
}

int main() {

        std::cout << "matt: " << std::bitset<64>(cubepos::mask_syll[48]) << std::endl;

	if (lrand48() == 0)
		srand48(time(0));

	pack_unpack();
	move_cp();
	move_cp_sc();

	return 0;
}
