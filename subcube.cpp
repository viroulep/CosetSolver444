#include <iostream>
#include "subcube.h"
#include "coset.h"

void subgroup::init(){
  initSym2Raw();
  initMove();
}

/* Unpack a centerR coord to a cube */
void subcube::unpack_center_r(cubepos cube, unsigned short center_r)
{
  unsigned char udfbl = 0;
  int r = 4;
  for (int i = 23; i >= 0; --i) {
    if (center_raw < cube.Cnk[i][r] ) {
      cube.centers[i] = udfbl++/4;
    } else {
      center_raw -= cube.Cnk[i][r--];
      cube.centers[i] = 5;
    }
  }
}

/* Pack a cube into the raw center coord */
unsigned short subcube::pack_center_r(cubepos cube){
  unsigned short center = 0;
  int r = 4;
  for (int i = 23; i >= 0; i--) {
    if (cube.centers[i] == 5) {
      center_raw += cube.Cnk[i][r--];
    }
  }
  return center;
}

/* Unpack an raw edge coord to a cube */
void subcube::unpack_edge(cubepos cube, unsigned int edge_raw)
{
  unsigned char r_edge = 0;
  unsigned char l_edge = 12;
  int r = 12;
  for (int i = 23; i >= 0; --i) {
    if (edge_raw < cubepos.Cnk[i][r] ) {
      cube.edges[i] = l_edge++;
    } else {
      center_raw -= cubepos.Cnk[i][r--];
      cube.edges[i] = r_edge++;
    }
  }
}

/* Pack a cube into the raw edge coord */
unsigned int subcube::pack_edge(cubepos cube){
  unsigned short edge = 0;
  int r = 12;
  unsigned short last_edge = cube.edges[23];
  for (int i = 23; i >= 0; i--) {
    if ((last_edge < 12) != (cube.edges[i] < 12)) {
      edge += cube.Cnk[i][r--];
    }
  }
  return edge;
}

void subcube::initSym2Raw (){
  int repIdx = 0;
  cubepos cube1;
  cubepos cube2;

  unsigned char isRepTable[(N_RAW_COORD_EDGES>>3) + 1];
  for (int u = 0; u < N_RAW_COORD_EDGES; ++u) {
    if (get1bit(isRepTable, u)) continue;
    raw2sym[u] = repIdx << SYM_SHIFT;
    unpack_edge(cube1, u);
    for (int s = 1; s < N_SYM; ++s) {
      cube1.conjugate (s, cube2);
      unsigned int raw_coord = pack_edge(cube2);
      set1bit( isRepTable, raw_coord );
      raw2sym[raw_coord] = ( repIdx << SYM_SHIFT ) + cube1.invSymIdx[s];
      if( raw_coord == u )
        hasSym[repIdx] |= (1 << s);
    }
    sym2raw[repIdx++] = u;
  }
  if(repIdx != N_COORD_EDGES) {
    std::cout << "The number of sym coordinates is not correct: " << repIdx;
  }
}

/* Pack all coordinates from the full cube position, and compute the sym coordinate */
void subcube::pack_all(cubepos cube){
  center_r = pack_center_r(cube);
  center_fb = pack_center_fb(cube);
  edge_raw = pack_edge(cube);
  edge_sym = raw2sym[edge_raw];
  sym = edge_sym & SYM_MASK;
  edge_sym >>= SYM_SHIFT;
}

/* Initialise all move and conjugate arrays */
void subcube::initMove (){
  cubepos cube1;
  cubepos cube2;

  /* Initialise edges move array */  
  for (int u = 0; u < N_COORD_EDGES; ++u) {
    unpack_edge(cube1, sym2raw[u]);
    for (int m = 0; m < N_MOVES; ++m) {
      cube2 = cube1;
      cube2.move (cubepos.stage2moves[m]);
      moveTableEdge[u][m] = raw2sym[pack_edge(cube2)];
    }
  }

  /* Initialise centerR move and conj arrays */
  for (int u = 0; u < N_COORD_CENTER_R; ++u) {
    unpack_center_r(cube1, u);
    for (int m = 0; m < N_MOVES; ++m) {
      cube2 = cube1;
      cube2.move (cubepos.stage2moves[m]);
      moveTableCenterR[u][m] = pack_center_r(cube2);
    }
    for (int s = 0; s < N_SYM; ++s) {
      cube1.conjugate (s, cube2);
      conjTableCenterR[u][s] = pack_center_r(cube2);
    }
  }

  /* Initialise centerFB move and conj arrays */
  coset c; // We can reuse the pack and unpack methods of the coset class.
  for (int u = 0; u < N_COORD_CENTER_FB; ++u) {
    c.unpack(cube1, u);
    for (int m = 0; m < N_MOVES; ++m) {
      cube2 = cube1;
      cube2.move (cubepos.stage2moves[m]);
      moveTableCenterFB[u][m] = c.pack(cube2);
    }
    for (int s = 0; s < N_SYM; ++s) {
      cube1.conjugate (s, cube2);
      conjTableCenterFB[u][s] = c.pack(cube2);
    }
  }
}

/* Apply a move to the subgroup state */
void subgroup::moveTo( int m, subgroup c ){
  c.center_r = moveTableCenterR[center_r][m];
  c.center_fb = moveTableCenterFB[center_fb][m];
  c.edge_sym = moveTableEdge[edge_sym][cubepos.moveConjugateStage[m][sym]];
  c.sym = cubepos.symIdxMultiply[c.edge_sym & SYM_MASK][sym];
  c.edge_sym >>>= SYM_SHIFT;
}

/* Get the canon state without any symmetry applied */
void subgroup::canonize( int sym ){
  center_r = conjTableCenterR[center_r][sym];
  center_fb = conjTableCenterFB[center_fb][sym];
  sym = 0;
}
