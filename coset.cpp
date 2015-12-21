#include "coset.h"
#include <iostream>
#include <iomanip>

/* Definition of static variables (more c++ random stuff) */
unsigned int coset::sym2raw[COSET_N_COORD];
unsigned int coset::raw2sym[COSET_N_RAW_COORD];
unsigned long long coset::hasSym[COSET_N_COORD] = {0};
unsigned int coset::moveTable[COSET_N_COORD][COSET_N_MOVES];
unsigned char coset::ptable[COSET_N_COORD>>1];

void coset::init(){
  static int initialized = 0;
  if (initialized)
    return;
  initialized = 1;
  
  std::cout << "coset: initSym2Raw" << std::endl;
  initSym2Raw();
  std::cout << "coset: initMove" << std::endl;
  initMove();
  std::cout << "coset: fillPruningTable" << std::endl;
  fillPruningTable();
}

/* Unpack a raw center coord to a cube */
void coset::unpack(cubepos &cube, unsigned int center_raw)
{
  unsigned char udfb = 0;
  unsigned char rl = 16;
  int r = 8;
  for (int i = 23; i >= 0; i--) {
    if (center_raw < cubepos::Cnk[i][r] ) {
      cube.centers[i] = udfb++/4;
    } else {
      center_raw -= cubepos::Cnk[i][r--];
      cube.centers[i] = rl++/4;
    }
  }
}

/* Pack a cube into the raw center coord */
unsigned int coset::pack(const cubepos &cube){
  unsigned int center_raw = 0;
  int r = 8;
  for (int i = 23; i >= 0; i--) {
    if (cube.centers[i] >= 4) {
      center_raw += cubepos::Cnk[i][r--];
    }
  }
  return center_raw;
}

void coset::initSym2Raw (){
  int repIdx = 0;
  cubepos cube1;
  cubepos cube2;

  unsigned char isRepTable[(COSET_N_RAW_COORD>>3) + 1] = {0};
  for (unsigned int u = 0; u < COSET_N_RAW_COORD; ++u) {
    //std::cout << "Coord " << u << std::endl;
    if (get1bit(isRepTable, u)) continue;
    raw2sym[u] = repIdx << COSET_SYM_SHIFT;
    unpack(cube1, u);
    for (int s = 1; s < COSET_N_SYM; ++s) {
      //cube1.conjugate (s, cube2);
      cube1.rightMult( cubepos::invSymIdx[s], cube2 );
      unsigned int raw_coord = pack(cube2);
      //std::cout << "Store " << raw_coord << " for sym " << s << std::endl;
      set1bit( isRepTable, raw_coord );
      raw2sym[raw_coord] = ( repIdx << COSET_SYM_SHIFT ) + cubepos::invSymIdx[s];
      if( raw_coord == u )
        hasSym[repIdx] |= (0x1L << s);
    }
    sym2raw[repIdx++] = u;
  }
  if(repIdx != COSET_N_COORD) {
    std::cout << "The number of sym coordinates is not correct: " << repIdx << std::endl;
  }
}

/* Pack all coordinates from the full cube position, and compute the sym coordinate */
void coset::pack_all(const cubepos &cube){
  center_rl_raw = pack(cube);
  center_rl_sym = raw2sym[center_rl_raw];
  sym = center_rl_sym & COSET_SYM_MASK;
  center_rl_sym >>= COSET_SYM_SHIFT;
}

void coset::initMove (){
  cubepos cube1;
  cubepos cube2;
  
  for (int u = 0; u < COSET_N_COORD; ++u) {
    unpack(cube1, sym2raw[u]);
    for (int m = 0; m < COSET_N_MOVES; ++m) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTable[u][m] = raw2sym[pack(cube2)];
    }
  }
}

void coset::moveTo( int m, coset &c ){
  unsigned int newcenter = moveTable[center_rl_sym][cubepos::moveConjugateStage[m][sym]];
  c.center_rl_sym = (unsigned short) (newcenter >> COSET_SYM_SHIFT);
  c.sym = cubepos::symIdxMultiply[newcenter & COSET_SYM_MASK][sym];
}

void coset::fillPruningTable(){

  int psize = COSET_N_COORD; // Size of the distance table

  /* Start filling the table with -1 */
  for (int i=0; i<psize>>1; i++)
    ptable[i] = 0xff;

  int pos = 0; // Number of positions for a given depth
  int total_pos = 0; // Total number of positions
  int unique = 0; // Number of unique positions for a given depth
  int total_unique = 0; // Total number of unique positions
  int done = 0; // Number of written values in the distance table

  /* Add the 0-distance for solved positions */
  int solvedState = 0;
  writeTable(solvedState, 0);
  done++;
  unique++;

  /* Count symmetric positions */
  int nsym = 1;
  unsigned long long symS = coset::hasSym[solvedState];
  for (int k=0; symS != 0; symS>>=1, k++) {
    if ((symS & 0x1L) == 0) continue;
    nsym++;
  }
  pos = COSET_N_SYM/nsym;

  total_pos += pos;
  total_unique += unique;
  std::cout << std::setw(2) << 0 << std::setw(12) << pos << std::setw(10) << unique << std::endl;

  /* Build the table */
  coset c;
  int depth = 0;
  while (( done < psize ) && ( depth < 15 )) {
    depth++;
    pos = 0;
    unique = 0;
    for (int i=0; i<psize; i++) {
      if (readTable(i) != (depth-1)) continue;

      /* Set our coset with id i */
      center_rl_sym = i;
      sym = 0;

      /* We apply each move to our coset */
      for (int m=0; m<COSET_N_MOVES; m++) {
        moveTo(m, c);
        int idx = c.center_rl_sym;
        if (readTable(idx) != 0x0f) continue;

        /* Storing this new position in the distance table */
        done++;
        writeTable(idx, depth);
        nsym = 1;
        unique++;

        /* Counting symmetries in our position */
        unsigned long long symS = coset::hasSym[idx];
        for (int k=0; symS != 0; symS>>=1, k++) {
          if ((symS & 0x1L) == 0) continue;
          nsym++;
        }
        pos += COSET_N_SYM/nsym;
      }
    }
    total_pos += pos;
    total_unique += unique;
    std::cout << std::setw(2) << depth << std::setw(12) << pos << std::setw(10) << unique << std::endl;
  }
  std::cout << "-- ----------- ---------" << std::endl;
  std::cout << std::setw(14) << total_pos << std::setw(10) << total_unique << std::endl;
}
