#ifndef CUBEPOS_H
#define CUBEPOS_H

#include <cstring>

#define MOVE_Uf1  0  //Up "face" (top slice) clockwise wrt top
#define MOVE_Uf2  1  //Up "face" counter-clockwise
#define MOVE_Uf3  2  //Up "face" 180 degrees

#define MOVE_Us1  3  //Up "slice" (upper inner slice) clockwise wrt top
#define MOVE_Us2  4  //Up "slice" counter-clockwise
#define MOVE_Us3  5  //Up "slice" 180 degrees

#define MOVE_Uw1  6  //Up "wide" (double outer slice) clockwise wrt top
#define MOVE_Uw2  7  //Up "wide" counter-clockwise
#define MOVE_Uw3  8  //Up "wide" 180 degrees

#define MOVE_Df1  9  //Down "face" (bottom slice) clockwise wrt bottom
#define MOVE_Df2  10 //Down "face" counter-clockwise
#define MOVE_Df3  11 //Down "face" 180 degrees

#define MOVE_Ds1  12 //Down "slice" (lower inner slice) clockwise wrt bottom
#define MOVE_Ds2  13 //Down "slice" counter-clockwise
#define MOVE_Ds3  14 //Down "slice" 180 degrees

#define MOVE_Dw1  15 //Down "wide" (double outer slice) clockwise wrt bottom
#define MOVE_Dw2  16 //Down "wide" counter-clockwise
#define MOVE_Dw3  17 //Down "wide" 180 degrees

#define MOVE_Lf1  18 //Left "face" (left-hand outer slice) clockwise wrt left side
#define MOVE_Lf2  19 //Left "face" counter-clockwise
#define MOVE_Lf3  20 //Left "face" 180 degrees

#define MOVE_Ls1  21 //Left "slice" (left-hand inner slice) clockwise wrt left side
#define MOVE_Ls2  22 //Left "slice" counter-clockwise
#define MOVE_Ls3  23 //Left "slice" 180 degrees

#define MOVE_Lw1  24 //Left "wide" (left-hand double outer slice) clockwise wrt left side
#define MOVE_Lw2  25 //Left "wide" counter-clockwise
#define MOVE_Lw3  26 //Left "wide" 180 degrees

#define MOVE_Rf1  27 //Right "face" (right-hand outer slice) clockwise wrt right side
#define MOVE_Rf2  28 //Right "face" counter-clockwise
#define MOVE_Rf3  29 //Right "face" 180 degrees

#define MOVE_Rs1  30 //Right "slice" (right-hand inner slice) clockwise wrt right side
#define MOVE_Rs2  31 //Right "slice" counter-clockwise
#define MOVE_Rs3  32 //Right "slice" 180 degrees

#define MOVE_Rw1  33 //Right "wide" (right-hand double outer slice) clockwise wrt right side
#define MOVE_Rw2  34 //Right "wide" counter-clockwise
#define MOVE_Rw3  35 //Right "wide" 180 degrees

#define MOVE_Ff1  36 //Front "face" (front outer slice) clockwise wrt front
#define MOVE_Ff2  37 //Front "face" counter-clockwise
#define MOVE_Ff3  38 //Front "face" 180 degrees

#define MOVE_Fs1  39 //Front "slice" (front inner slice) clockwise wrt front
#define MOVE_Fs2  40 //Front "slice" counter-clockwise
#define MOVE_Fs3  41 //Front "slice" 180 degrees

#define MOVE_Fw1  42 //Front "wide" (front double outer slice) clockwise wrt front
#define MOVE_Fw2  43 //Front "wide" counter-clockwise
#define MOVE_Fw3  44 //Front "wide" 180 degrees

#define MOVE_Bf1  45 //Back "face" (rear outer slice) clockwise wrt back side
#define MOVE_Bf2  46 //Back "face" counter-clockwise
#define MOVE_Bf3  47 //Back "face" 180 degrees

#define MOVE_Bs1  48 //Back "slice" (rear inner slice) clockwise wrt back side
#define MOVE_Bs2  49 //Back "slice" counter-clockwise
#define MOVE_Bs3  50 //Back "slice" 180 degrees

#define MOVE_Bw1  51 //Back "wide" (rear double outer slice) clockwise wrt back side
#define MOVE_Bw2  52 //Back "wide" counter-clockwise
#define MOVE_Bw3  53 //Back "wide" 180 degrees

#define N_MOVES  MOVE_Bw3+1 //last rotate code plus one

#define N_STAGE_MOVES 36
#define N_SYM 48

/*
Edges
------

There are 24 "edge" cubies, numbered 0 to 23.
The home positions of these cubies are labeled in the diagram below.
Each edge cubie has two exposed faces, so there are two faces labelled with each number.

                -------------
                |   14  2   |
                | 1   U   15|
                |13        3|
                |    0 12   |
    -------------------------------------------------
    |    1 13   |    0 12   |    3 15   |    2 14   |
    | 9   L   20|20   F   11|11   R   22|22   B    9|
    |21        8| 8       23|23       10|10       21|
    |   17  5   |   18  6   |   19  7   |   16  4   |
    -------------------------------------------------
                |   18  6   |
                | 5   D   19|
                |17        7|
                |    4 16   |
                -------------

Centers
-------

There are 24 "center" cubies. They are numbered 0 to 23 as shown.

                -------------
                |           |
                |    3  1   |
                |    0  2   |
                |           |
    -------------------------------------------------
    |           |           |           |           |
    |   16 19   |   10  8   |   21 22   |   14 12   |
    |   18 17   |    9 11   |   23 20   |   13 15   |
    |           |           |           |           |
    -------------------------------------------------
                |           |
                |    6  4   |
                |    5  7   |
                |           |
                -------------

*/


class cubepos {
	public :
		unsigned char centers[24];
		unsigned char edges[24];

		inline bool operator==(const cubepos &cp) const {
			return std::memcmp(this, &cp, sizeof(cp)) == 0;
		}

		inline bool operator!=(const cubepos &cp) const {
			return std::memcmp(this, &cp, sizeof(cp)) != 0;
		}

		inline cubepos& operator=(const cubepos &cp) {
			std::memcpy(edges, cp.edges, 24);
			std::memcpy(centers, cp.centers, 24);
			return *this;
		  }

		/* Constructors */
		cubepos();
		cubepos(int,int,int);

    	/* Convert general move notation to stage move notation (ordered so that moves for each stage are (1..N_s)) */
	    static int stage2moves[N_STAGE_MOVES]; 
		static int moves2stage[N_MOVES]; // Inverse of the above array

		/* Stores how each symmetry is permuting pieces of the cube */
		static unsigned char symEdges[N_SYM][24];
		static unsigned char symCenters[N_SYM][24];

		/* Stores some arithmetic of symmetries */
		static int invSymIdx[N_SYM]; // Inverse of a symmetry
		static int symIdxMultiply[N_SYM][N_SYM]; // Composition of two symmetries
		static int moveConjugate[N_MOVES][N_SYM]; // Mapping of moves by symmetries
		static int moveConjugateStage[N_STAGE_MOVES][N_SYM]; // Mapping of moves by symmetries in the stage space

		static unsigned int Cnk[25][25]; // binomial coefficients

		/* Perform the permutation cycle (a b c d) on the tab array */
		static inline void cycle(unsigned char tab[], int a, int b, int c, int d, int times){
			unsigned char temp = tab[d];
			tab[d] = tab[c];
			tab[c] = tab[b];
			tab[b] = tab[a];
			tab[a] = temp;
			if(times > 0)
				cycle(tab, a, b, c, d, times - 1);
		}

		void init();
		void identity();
		void move(int move);
		void initSymTables();
		void initInvSymIdx();
		void initSymIdxMultiply();
		void leftMult(int symIdx);
		void rightMult(int symIdx, cubepos &c);
		void conjugate (int symIdx, cubepos &c);
		void initMoveConjugate();
		void initCnk();
};

//static cubepos cubepos_initialization_hack(1,2,3);

#endif
