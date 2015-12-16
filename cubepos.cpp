#include "cubepos.h"

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
	initSymTables();
	initInvSymIdx();
	initSymIdxMultiply();
	initMoveConjugate();
	initCnk();
}

/* Set the cube as the solved state */
void cubepos::identity() {
	for (unsigned char i = 0; i < 24; ++i) {
		centers[i] = i/4;
		edges[i] = i;
	}
}

/* Apply a single move to the cube position */
void cubepos::move(int move) {
	int rot = (move % 3) + 1;
	int layer = move / 3;
	switch (layer) {
		case 0: // U
			cycle(centers, 0, 3, 1, 2, rot);
			cycle(edges, 0, 12, 1, 14, rot);
			cycle(edges, 4, 8, 5, 10, rot);
			break;
		case 1: // u
			cycle(centers, 16, 10, 21, 14, rot);
			cycle(centers, 19, 8, 22, 12, rot);
			cycle(edges, 16, 22, 19, 21, rot);
			break;
		case 2: // Uw
			cycle(centers, 0, 3, 1, 2, rot);
			cycle(centers, 16, 10, 21, 14, rot);
			cycle(centers, 19, 8, 22, 12, rot);
			cycle(edges, 0, 12, 1, 14, rot);
			cycle(edges, 4, 8, 5, 10, rot);
			cycle(edges, 16, 22, 19, 21, rot);
			break;
		case 3: // D
			cycle(centers, 4, 7, 5, 6, rot);
			cycle(edges, 2, 15, 3, 13, rot);
			cycle(edges, 6, 11, 7, 9, rot);
			break;
		case 4: // d
			cycle(centers, 18, 13, 23, 9, rot);
			cycle(centers, 17, 15, 20, 11, rot);
			cycle(edges, 17, 23, 18, 20, rot);
			break;
		case 5: // Dw
			cycle(centers, 4, 7, 5, 6, rot);
			cycle(centers, 18, 13, 23, 9, rot);
			cycle(centers, 17, 15, 20, 11, rot);
			cycle(edges, 2, 15, 3, 13, rot);
			cycle(edges, 6, 11, 7, 9, rot);
			cycle(edges, 17, 23, 18, 20, rot);
			break;
		case 6: // L
			cycle(centers, 8, 11, 9, 10, rot);
			cycle(edges, 8, 20, 9, 22, rot);
			cycle(edges, 12, 16, 13, 18, rot);
			break;
		case 7: // l
			cycle(centers, 16, 6, 20, 3, rot);
			cycle(centers, 18, 5, 22, 0, rot);
			cycle(edges, 0, 6, 3, 5, rot);
			break;
		case 8: // Lw
			cycle(centers, 8, 11, 9, 10, rot);
			cycle(centers, 16, 6, 20, 3, rot);
			cycle(centers, 18, 5, 22, 0, rot);
			cycle(edges, 8, 20, 9, 22, rot);
			cycle(edges, 12, 16, 13, 18, rot);
			cycle(edges, 0, 6, 3, 5, rot);
			break;
		case 9: // R
			cycle(centers, 12, 15, 13, 14, rot);
			cycle(edges, 10, 23, 11, 21, rot);
			cycle(edges, 14, 19, 15, 17, rot);
			break;
		case 10: // r
			cycle(centers, 19, 1, 23, 4, rot);
			cycle(centers, 17, 2, 21, 7, rot);
			cycle(edges, 1, 7, 2, 4, rot);
			break;
		case 11: // Rw
			cycle(centers, 12, 15, 13, 14, rot);
			cycle(centers, 19, 1, 23, 4, rot);
			cycle(centers, 17, 2, 21, 7, rot);
			cycle(edges, 10, 23, 11, 21, rot);
			cycle(edges, 14, 19, 15, 17, rot);
			cycle(edges, 1, 7, 2, 4, rot);
			break;
		case 12: // F
			cycle(centers, 16, 19, 17, 18, rot);
			cycle(edges, 0, 21, 2, 20, rot);
			cycle(edges, 4, 17, 6, 16, rot);
			break;
		case 13: // f
			cycle(centers, 0, 14, 4, 11, rot);
			cycle(centers, 2, 13, 6, 8, rot);
			cycle(edges, 8, 14, 11, 13, rot);
			break;
		case 14: // Fw
			cycle(centers, 16, 19, 17, 18, rot);
			cycle(centers, 0, 14, 4, 11, rot);
			cycle(centers, 2, 13, 6, 8, rot);
			cycle(edges, 0, 21, 2, 20, rot);
			cycle(edges, 4, 17, 6, 16, rot);
			cycle(edges, 8, 14, 11, 13, rot);
			break;
		case 15: // B
			cycle(centers, 20, 23, 21, 22, rot);
			cycle(edges, 1, 22, 3, 23, rot);
			cycle(edges, 5, 18, 7, 19, rot);
			break;
		case 16: // b
			cycle(centers, 1, 10, 5, 15, rot);
			cycle(centers, 3, 9, 7, 12, rot);
			cycle(edges, 9, 15, 10, 12, rot);
			break;
		case 17: // Bw
			cycle(centers, 20, 23, 21, 22, rot);
			cycle(centers, 1, 10, 5, 15, rot);
			cycle(centers, 3, 9, 7, 12, rot);
			cycle(edges, 1, 22, 3, 23, rot);
			cycle(edges, 5, 18, 7, 19, rot);
			cycle(edges, 9, 15, 10, 12, rot);
			break;
	}
}

/* Initialise the arrays of permutations of the different symmetries */
void cubepos::initSymTables(){

	static const unsigned char symRLEdges[24] = {4, 5, 6, 7, 0, 1, 2, 3, 14, 15, 12, 13, 10, 11, 8, 9, 21, 20, 23, 22, 17, 16, 19, 18};
	static const unsigned char symRLCenters[24] = {2, 3, 0, 1, 6, 7, 4, 5, 14, 15, 12, 13, 10, 11, 8, 9, 19, 18, 17, 16, 23, 22, 21, 20};

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
void cubepos::rightMult (int symIdx, cubepos c){
	for (int i = 0; i < 24; i++){
		c.centers[i] = centers[symCenters[symIdx][i]];
		c.edges[i] = edges[symEdges[symIdx][i]];
	}
}

/* Conjugate the cube state by a symmetry */
void cubepos::conjugate (int symIdx, cubepos c){
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