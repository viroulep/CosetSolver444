#include "coset.h"
#include <iostream>

/* Unpack a raw center coord to a cube */
void coset::unpack(cubepos cube, unsigned int center_raw)
{
  unsigned char udfb = 0;
  unsigned char rl = 0;
  int r = 8;
  for (int i = 23; i >= 0; --i) {
    if (center_raw < cube.Cnk[i][r] ) {
      cube.centers[i] = udfb++/4;
    } else {
      center_raw -= cube.Cnk[i][r--];
      cube.centers[i] = rl++/4;
    }
  }
}

/* Pack a cube into the raw center coord */
unsigned int coset::pack(cubepos cube){
  unsigned int center_raw = 0;
  int r = 8;
  for (int i = 23; i >= 0; i--) {
    if (cube.centers[i] >= 4) {
      center_raw += cube.Cnk[i][r--];
    }
  }
  return center_raw;
}

void coset::initSym2Raw (){
  sym2raw = new int[N_COORD];
  raw2sym = new int[N_RAW_COORD];
  int repIdx = 0;
  cubepos cube1;
  cubepos cube2;

  unsigned char isRepTable[(N_RAW_COORD>>3) + 1];
  for (int u = 0; u < N_RAW_COORD; ++u) {
    if (get1bit(isRepTable, u)) continue;
    raw2sym[u] = repIdx << SYM_SHIFT;
    unpack(cube1, u);
    for (int s = 1; s < N_SYM; ++s) {
      cube1.rightMult (cube1.invSymIdx[s], cube2);
      cube2.leftMult (s);
      unsigned int raw_coord = pack(cube2);
      set1bit( isRepTable, raw_coord );
      raw2sym[raw_coord] = ( repIdx << SYM_SHIFT ) + cube1.invSymIdx[s];
      if( raw_coord == u )
        hasSym[repIdx] |= (0x1L << s);
    }
    sym2raw[repIdx++] = u;
  }
  if(repIdx != N_COORD) {
    std::cout << "The number of sym coordinates is not correct: " << repIdx;
  }
}

void coset::initMove (){
  cubepos cube1;
  cubepos cube2;
  
  for (int u = 0; u < N_COORD; ++u) {
    unpack(cube1, sym2raw[u]);
    for (int m = 0; m < N_MOVES; ++m) {
      cube2 = cube1;
      cube2.move (cube2.stage2moves[m]);
      move[u][m] = raw2sym[pack(cube2)];
    }
  }
}
