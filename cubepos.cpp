#include "cubepos.h"
#include <iostream>

/* Definition of static variables (more c++ random stuff) */
int cubepos::stage2moves[N_STAGE_MOVES] = {
	MOVE_Uf1, MOVE_Uf2, MOVE_Uf3, MOVE_Rf2, MOVE_Ff1, MOVE_Ff2, MOVE_Ff3, 
	MOVE_Df1, MOVE_Df2, MOVE_Df3, MOVE_Lf2, MOVE_Bf1, MOVE_Bf2, MOVE_Bf3, 
	MOVE_Us2, MOVE_Rs2, MOVE_Fs2, MOVE_Ds2, MOVE_Ls2, MOVE_Bs2, // Stage 3 moves
	MOVE_Rf1, MOVE_Rf3, MOVE_Lf1, MOVE_Lf3, MOVE_Rs1, MOVE_Rs3, MOVE_Ls1, MOVE_Ls3, // Stage 2/3 moves
	MOVE_Us1, MOVE_Us3, MOVE_Fs1, MOVE_Fs3, MOVE_Ds1, MOVE_Ds3, MOVE_Bs1, MOVE_Bs3 // Stage 1/2/3 moves
};
int cubepos::moves2stage[N_MOVES];
unsigned char cubepos::symEdges[N_SYM][24];
unsigned char cubepos::symCenters[N_SYM][24];
int cubepos::invSymIdx[N_SYM];
int cubepos::symIdxMultiply[N_SYM][N_SYM];
int cubepos::moveConjugate[N_MOVES][N_SYM];
int cubepos::moveConjugateStage[N_STAGE_MOVES][N_SYM];
int cubepos::Cnk[25][25];

/* Initialise static arrays and stuff */
void cubepos::init() {
	static int initialized = 0;
	if (initialized)
		return;
	initialized = 1;

	std::cout << "cubepos: initSymTables" << std::endl;
	initSymTables();
	std::cout << "cubepos: initInvSymIdx" << std::endl;
	initInvSymIdx();
	std::cout << "cubepos: initSymIdxMultiply" << std::endl;
	initSymIdxMultiply();
	std::cout << "cubepos: initMoveConjugate" << std::endl;
	initMoveConjugate();
	std::cout << "cubepos: initCnk" << std::endl;
	initCnk();
}

/* Set the cube as the solved state */
void cubepos::identity() {
	for (unsigned char i = 0; i < 24; ++i) {
		centers[i] = i/4;
		edges[i] = i;
	}
}

cubepos::cubepos(int,int,int) {
	init();
}

cubepos::cubepos() {
	identity();
}

/* Apply a single move to the cube position */
void cubepos::move(int move) {
	int rot = move % 3;
	int layer = move / 3;
	switch (layer) {
		case 0: // U
			cycle(centers, 0, 3, 1, 2, rot);
			cycle(edges, 0, 1, 2, 3, rot);
			cycle(edges, 12, 13, 14, 15, rot);
			break;
		case 1: // u
			cycle(centers, 10, 16, 14, 21, rot);
			cycle(centers, 8, 19, 12, 22, rot);
			cycle(edges, 11, 20, 9, 22, rot);
			break;
		case 2: // Uw
			cubepos::move(MOVE_Uf1+rot);
			cubepos::move(MOVE_Us1+rot);
			break;
		case 3: // D
			cycle(centers, 4, 7, 5, 6, rot);
			cycle(edges, 4, 5, 6, 7, rot);
			cycle(edges, 16, 17, 18, 19, rot);
			break;
		case 4: // d
			cycle(centers, 18, 9, 23, 13, rot);
			cycle(centers, 17, 11, 20, 15, rot);
			cycle(edges, 8, 23, 10, 21, rot);
			break;
		case 5: // Dw
			cubepos::move(MOVE_Df1+rot);
			cubepos::move(MOVE_Ds1+rot);
			break;
		case 6: // L
			cycle(centers, 16, 19, 17, 18, rot);
			cycle(edges, 1, 20, 5, 21, rot);
			cycle(edges, 13, 8, 17, 9, rot);
			break;
		case 7: // l
			cycle(centers, 10, 6, 15, 3, rot);
			cycle(centers, 9, 5, 12, 0, rot);
			cycle(edges, 0, 18, 4, 14, rot);
			break;
		case 8: // Lw
			cubepos::move(MOVE_Lf1+rot);
			cubepos::move(MOVE_Ls1+rot);
			break;
		case 9: // R
			cycle(centers, 20, 23, 21, 22, rot);
			cycle(edges, 3, 22, 7, 23, rot);
			cycle(edges, 15, 10, 19, 11, rot);
			break;
		case 10: // r
			cycle(centers, 8, 1, 13, 4, rot);
			cycle(centers, 11, 2, 14, 7, rot);
			cycle(edges, 2, 16, 6, 12, rot);
			break;
		case 11: // Rw
			cubepos::move(MOVE_Rf1+rot);
			cubepos::move(MOVE_Rs1+rot);
			break;
		case 12: // F
			cycle(centers, 8, 11, 9, 10, rot);
			cycle(edges, 0, 11, 6, 8, rot);
			cycle(edges, 12, 23, 18, 20, rot);
			break;
		case 13: // f
			cycle(centers, 0, 21, 4, 17, rot);
			cycle(centers, 2, 23, 6, 19, rot);
			cycle(edges, 3, 19, 5, 13, rot);
			break;
		case 14: // Fw
			cubepos::move(MOVE_Ff1+rot);
			cubepos::move(MOVE_Fs1+rot);
			break;
		case 15: // B
			cycle(centers, 12, 15, 13, 14, rot);
			cycle(edges, 2, 9, 4, 10, rot);
			cycle(edges, 14, 21, 16, 22, rot);
			break;
		case 16: // b
			cycle(centers, 1, 16, 5, 20, rot);
			cycle(centers, 3, 18, 7, 22, rot);
			cycle(edges, 1, 17, 7, 15, rot);
			break;
		case 17: // Bw
			cubepos::move(MOVE_Bf1+rot);
			cubepos::move(MOVE_Bs1+rot);
			break;
	}
}

/* Initialise the arrays of permutations of the different symmetries */
void cubepos::initSymTables(){

	static const unsigned char symRLEdges[24] = {12, 15, 14, 13, 16, 19, 18, 17, 23, 22, 21, 20, 0, 3, 2, 1, 4, 7, 6, 5, 11, 10, 9, 8};
	static const unsigned char symRLCenters[24] = {2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13, 22, 23, 20, 21, 18, 19, 16, 17};

	int i, b, c, d, e, idx=0;
	identity();
	for (i = 0; i < 24; ++i)
		centers[i] = (unsigned char)i; // We need to consider the cube as a super cube (unique centers) for the following

	for (b=0;b<3;b++){ //SymUR3
		for (c=0;c<2;c++){ //SymU
			for (d=0;d<2;d++){ //SymF2
				for (e=0;e<2;e++){ //SymU2
					//SymLR2
					for (i=0; i<24; i++){
						symEdges[idx][i] = edges[i];
						symEdges[idx+4][i] = edges[symRLEdges[i]];
						symCenters[idx][i] = centers[i];
						symCenters[idx+4][i] = centers[symRLCenters[i]];
					}
					idx += 1;
					move (MOVE_Uw2);
					move (MOVE_Dw2);
				}
				move (MOVE_Fw2);
				move (MOVE_Bw2);
			}
			idx += 4;
			move (MOVE_Uw1);
			move (MOVE_Dw3);
		}
		move (MOVE_Uw3);
		move (MOVE_Dw1);
		move (MOVE_Rw3);
		move (MOVE_Lw1);
	}
}

/* Initialise the array storing the inverse of a symmetry */
void cubepos::initInvSymIdx(){
	for (int i=0; i<N_SYM; i++)
		for (int j=0; j<N_SYM; j++)
			if( symEdges[i][symEdges[j][0]] == 0 &&
				symEdges[i][symEdges[j][7]] == 7 &&
				symEdges[i][symEdges[j][18]] == 18 ){
				invSymIdx[i] = j; // We only check three edges
				break;
			}
}

/* Initialise the array storing the multiplication of two symmetries */
void cubepos::initSymIdxMultiply(){
	for (int i=0; i<N_SYM; i++)
		for (int j=0; j<N_SYM; j++)
			for (int k=0; k<N_SYM; k++)
				if( symEdges[k][0] == symEdges[i][symEdges[j][0]] &&
					symEdges[k][1] == symEdges[i][symEdges[j][1]] ){
					symIdxMultiply[i][j] = k;
					break;
				}
}

/* Multiply the cube state by a symmetry on the left */
void cubepos::leftMult (int symIdx){
	int cenN[6]; // Transform centers into unique facelets.
	for (int c = 0; c < 6; c++) cenN[c] = 0; // Initialise the array with zeros.

	for (int i = 0; i < 24; i++){
		centers[i] = symCenters[symIdx][centers[i]*4+cenN[centers[i]]++] / 4;
		edges[i] = symEdges[symIdx][edges[i]];
	}
}

/* Multiply the cube state by a symmetry on the right */
void cubepos::rightMult (int symIdx, cubepos &c){
	for (int i = 0; i < 24; i++){
		c.centers[i] = centers[symCenters[symIdx][i]];
		c.edges[i] = edges[symEdges[symIdx][i]];
	}
}

/* Conjugate the cube state by a symmetry */
void cubepos::conjugate (int symIdx, cubepos &c){
	rightMult( invSymIdx[symIdx], c );
	c.leftMult( symIdx );
}


void cubepos::initMoveConjugate(){

	cubepos cube;
	cubepos cube2;
	cubepos cube3;

	for (int i=0; i<N_MOVES; i++){
		cube.identity();
		cube.move(i);
		for (int j=0; j<N_SYM; j++){
			moveConjugate[i][j] = -1;
			cube.conjugate(j, cube2);
			for (int k=0; k<N_MOVES; k++){
				cube3.identity();
				cube3.move(k);
				char isMove = 1;
				for (int l=0; l<24; l++){
					if( cube3.edges[l] != cube2.edges[l] ){
						isMove = 0;
						break;
					}
				}
				if( isMove ){
					moveConjugate[i][j] = k;
					break;
				}
			}
			if (moveConjugate[i][j] == -1)
				std::cout << "Error in cubepos::initMoveConjugate: Could not find a conjugate move" << std::endl;
		}
	}

	for (int i=0; i<N_STAGE_MOVES; i++)
		moves2stage[stage2moves[i]] = i;

	for (int i=0; i<N_STAGE_MOVES; i++)
		for (int j=0; j<N_SYM; j++)
			moveConjugateStage[i][j] = moves2stage[moveConjugate[stage2moves[i]][j]];

}

/* We need to compute the binomial coefficients. */
void cubepos::initCnk() {
  for (int i=0; i<25; i++) {
    Cnk[i][i] = 1;
    Cnk[i][0] = 1;
  }
  for (int i=1; i<25; i++) {
    for (int j=1; j<=i; j++) {
      Cnk[i][j] = Cnk[i-1][j] + Cnk[i-1][j-1];
    }
  }
}

