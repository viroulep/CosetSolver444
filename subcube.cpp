#include <iostream>
#include "subcube.h"
#include "coset.h"

unsigned int subcube::sym2raw[SUBCUBE_N_COORD_EDGES];
unsigned int subcube::raw2sym[SUBCUBE_N_RAW_COORD_EDGES];
unsigned int subcube::hasSym[SUBCUBE_N_COORD_EDGES] = {0};
unsigned short subcube::moveTableCenterR[SUBCUBE_N_COORD_CENTER_R][SUBCUBE_N_MOVES_ALL];
unsigned int subcube::moveTableCenterFB[SUBCUBE_N_COORD_CENTER_FB][SUBCUBE_N_MOVES_ALL];
unsigned int subcube::moveTableEdge[SUBCUBE_N_COORD_EDGES][SUBCUBE_N_MOVES_ALL];
unsigned short subcube::conjTableCenterR[SUBCUBE_N_COORD_CENTER_R][SUBCUBE_N_MOVES_ALL];
unsigned int subcube::conjTableCenterFB[SUBCUBE_N_COORD_CENTER_FB][SUBCUBE_N_MOVES_ALL];
unsigned char subcube::centerRToSC[SUBCUBE_N_COORD_CENTER_R-SUBCUBE_MIN_CENTER_R_SC];
unsigned char subcube::moveTableCenterRSC[SUBCUBE_N_COORD_CENTER_R_SC][SUBCUBE_N_MOVES];
unsigned char subcube::conjTableCenterRSC[SUBCUBE_N_COORD_CENTER_R_SC][SUBCUBE_N_MOVES];
unsigned int subcube::reorientEdge[SUBCUBE_N_COORD_EDGES][2];
unsigned short subcube::reorientCenterR[SUBCUBE_N_COORD_CENTER_R][2];
unsigned int subcube::reorientCenterFB[SUBCUBE_N_COORD_CENTER_FB][2];


void subcube::init(){
  std::cout << "subcube: initSym2Raw" << std::endl;
  initSym2Raw();
  std::cout << "subcube: initMove" << std::endl;
  initMove();
  std::cout << "subcube: initToSC" << std::endl;
  initToSC();
  std::cout << "subcube: initMoveSC" << std::endl;
  initMoveSC();
  std::cout << "subcube: initReorientSC" << std::endl;
  initReorientSC();
}

/* Unpack a centerR coord to a cube */
void subcube::unpack_center_r(cubepos &cube, unsigned short center_raw)
{
  unsigned char udfbl = 0;
  int r = 4;
  for (int i = 23; i >= 0; i--) {
    if (center_raw < cubepos::Cnk[i][r] ) {
      cube.centers[i] = udfbl++/4;
    } else {
      center_raw -= cubepos::Cnk[i][r--];
      cube.centers[i] = 5;
    }
  }
}

/* Pack a cube into the raw center coord */
unsigned short subcube::pack_center_r(const cubepos &cube){
  unsigned short center = 0;
  int r = 4;
  for (int i = 23; i >= 0; i--) {
    if (cube.centers[i] == 5) {
      center += cubepos::Cnk[i][r--];
    }
  }
  return center;
}

/* Unpack a centerR coord inside RL faces (subcube) */
void subcube::unpack_center_r_sc(cubepos &cube, unsigned char center_raw)
{
  unsigned char udfb = 0;
  int r = 4;
  for (int i = 7; i >= 0; i--) {
    if (center_raw < cubepos::Cnk[i][r] ) {
      cube.centers[16+i] = 4;
    } else {
      center_raw -= cubepos::Cnk[i][r--];
      cube.centers[16+i] = 5;
    }
  }
  for (int i = 0; i < 16; i++)
      cube.centers[i] = udfb++/4;

}

/* Pack a subcube into the centerRL coord */
unsigned char subcube::pack_center_r_sc(const cubepos &cube){
  unsigned char center = 0;
  int r = 4;
  unsigned char last_center = cube.centers[23];
  for (int i = 7; i >= 0; i--) {
    if (cube.centers[16+i] != last_center) {
      center += cubepos::Cnk[i][r--];
    }
  }
  return center;
}

/* Unpack an raw edge coord to a cube */
void subcube::unpack_edge(cubepos &cube, unsigned int edge_raw)
{
  unsigned char r_edge = 0;
  unsigned char l_edge = 12;
  int r = 12;
  for (int i = 23; i >= 0; i--) {
    if (edge_raw < cubepos::Cnk[i][r] ) {
      cube.edges[i] = l_edge++;
    } else {
      edge_raw -= cubepos::Cnk[i][r--];
      cube.edges[i] = r_edge++;
    }
  }
}

/* Pack a cube into the raw edge coord */
unsigned int subcube::pack_edge(const cubepos &cube){
  unsigned int edge = 0;
  int r = 12;
  unsigned char last_edge = cube.edges[23];
  for (int i = 23; i >= 0; i--) {
    if ((last_edge < 12) != (cube.edges[i] < 12)) {
      edge += cubepos::Cnk[i][r--];
    }
  }
  return edge;
}

void subcube::initSym2Raw (){
  int repIdx = 0;
  cubepos cube1;
  cubepos cube2;

  unsigned char isRepTable[(SUBCUBE_N_RAW_COORD_EDGES>>3) + 1] = {0};
  for (unsigned int u = 0; u < SUBCUBE_N_RAW_COORD_EDGES; u++) {
    if (get1bit(isRepTable, u)) continue;
    raw2sym[u] = repIdx << SUBCUBE_SYM_SHIFT;
    unpack_edge(cube1, u);
    for (int s = 1; s < SUBCUBE_N_SYM; s++) {
      //cube1.conjugate (s, cube2);
      cube1.rightMult( cubepos::invSymIdx[s], cube2 );
      unsigned int raw_coord = pack_edge(cube2);
      set1bit( isRepTable, raw_coord );
      raw2sym[raw_coord] = ( repIdx << SUBCUBE_SYM_SHIFT ) + cubepos::invSymIdx[s];
      if( raw_coord == u )
        hasSym[repIdx] |= (1 << s);
    }
    sym2raw[repIdx++] = u;
  }
  if(repIdx != SUBCUBE_N_COORD_EDGES) {
    std::cout << "The number of sym coordinates is not correct: " << repIdx << std::endl;
  }
}

/* Pack all coordinates from the full cube position, and compute the sym coordinate */
void subcube::pack_all(const cubepos &cube){
  center_r = pack_center_r(cube);
  coset c;
  center_fb = c.pack(cube);
  edge_raw = pack_edge(cube);
  edge_sym = raw2sym[edge_raw];
  sym = edge_sym & SUBCUBE_SYM_MASK;
  edge_sym >>= SUBCUBE_SYM_SHIFT;
}

/* Initialise all move and conjugate arrays */
void subcube::initMove (){
  cubepos cube1;
  cubepos cube2;

  /* Initialise edges move array */  
  for (int u = 0; u < SUBCUBE_N_COORD_EDGES; ++u) {
    unpack_edge(cube1, sym2raw[u]);
    for (int m = 0; m < SUBCUBE_N_MOVES_ALL; ++m) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTableEdge[u][m] = raw2sym[pack_edge(cube2)];
    }
  }

  /* Initialise centerR move and conj arrays */
  for (int u = 0; u < SUBCUBE_N_COORD_CENTER_R; ++u) {
    unpack_center_r(cube1, u);
    for (int m = 0; m < SUBCUBE_N_MOVES_ALL; ++m) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTableCenterR[u][m] = pack_center_r(cube2);
    }
    for (int s = 0; s < SUBCUBE_N_SYM; ++s) {
      cube1.conjugate (s, cube2);
      conjTableCenterR[u][s] = pack_center_r(cube2);
    }
  }

  /* Initialise centerFB move and conj arrays */
  coset c; // We can reuse the pack and unpack methods of the coset class.
  for (int u = 0; u < SUBCUBE_N_COORD_CENTER_FB; ++u) {
    c.unpack(cube1, u);
    for (int m = 0; m < SUBCUBE_N_MOVES_ALL; ++m) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTableCenterFB[u][m] = c.pack(cube2);
    }
    for (int s = 0; s < SUBCUBE_N_SYM; ++s) {
      cube1.conjugate (s, cube2);
      conjTableCenterFB[u][s] = c.pack(cube2);
    }
  }
}

/* Apply a move to the subgroup state */
void subcube::moveTo( int m, subcube &c ){
  c.center_r = moveTableCenterR[center_r][m];
  c.center_fb = moveTableCenterFB[center_fb][m];
  c.edge_sym = moveTableEdge[edge_sym][cubepos::moveConjugateStage[m][sym]];
  c.sym = cubepos::symIdxMultiply[c.edge_sym & SUBCUBE_SYM_MASK][sym];
  c.edge_sym >>= SUBCUBE_SYM_SHIFT;
}

/* Get the canon state without any symmetry applied */
void subcube::canonize( int sym ){
  center_r = conjTableCenterR[center_r][sym];
  center_fb = conjTableCenterFB[center_fb][sym];
  sym = 0;
}

/* Initialise the conversion arrays to subcube coordinates */
void subcube::initToSC(){

	// Initialise the array with -1 to later catch bad conversions
	for (int c=0; c<SUBCUBE_N_COORD_CENTER_R-SUBCUBE_MIN_CENTER_R_SC; c++)
		centerRToSC[c] = 0xff;

	cubepos cp;
	for (unsigned short cr = 0; cr < 70; cr++) {
		unpack_center_r_sc(cp, cr);
		unsigned short old_center_r = pack_center_r(cp);
		unsigned short new_center_r = pack_center_r_sc(cp);
		if (old_center_r < SUBCUBE_MIN_CENTER_R_SC){
			std::cout << "Error in generating center_r subcube conversion: bad min" << std::endl;
			break;
		}
		centerRToSC[old_center_r - SUBCUBE_MIN_CENTER_R_SC] = new_center_r;
	}
}

/* Initialise move and conj arrays for centerR inside the subgropu */
void subcube::initMoveSC(){
  cubepos cube1;
  cubepos cube2;

  /* Initialise subcube centerR move and conj arrays */
  for (int u = 0; u < SUBCUBE_N_COORD_CENTER_R_SC; u++) {
    unpack_center_r_sc(cube1, u);
    for (int m = 0; m < SUBCUBE_N_MOVES; m++) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTableCenterRSC[u][m] = pack_center_r_sc(cube2);
    }
    for (int s = 0; s < SUBCUBE_N_SYM; s++) {
      cube1.conjugate (s, cube2);
      conjTableCenterRSC[u][s] = pack_center_r_sc(cube2);
    }
  }
}

/* After the first stage, the RL centers might not be in RL faces.
 * Thus we need to reorient the cube so that RL centers are in RL faces
 * so that we can solve inside the subgroup */
void subcube::initReorientSC(){

  cubepos cp1;
  cubepos cp2;

  for (int esym=0; esym<SUBCUBE_N_COORD_EDGES; esym++){
    unpack_edge(cp1, sym2raw[esym]);
    cp1.rightMult(16, cp2);
    reorientEdge[esym][0] = raw2sym[pack_edge(cp2)];
    cp1.rightMult(16, cp2);
    reorientEdge[esym][1] = raw2sym[pack_edge(cp2)];
  }

  for (int cr=0; cr<SUBCUBE_N_COORD_CENTER_R; cr++){
    unpack_center_r(cp1, cr);
    cp1.rightMult(16, cp2);
    reorientCenterR[cr][0] = pack_center_r(cp2);
    cp1.rightMult(16, cp2);
    reorientCenterR[cr][1] = pack_center_r(cp2);
  }

  /* We might not need to build this array, and use coset::conj instead. */
  coset c;
  for (int cfb=0; cfb<SUBCUBE_N_COORD_CENTER_FB; cfb++){
    c.unpack(cp1, cfb);
    cp1.rightMult(16, cp2);
    reorientCenterFB[cfb][0] = c.pack(cp2);
    cp1.rightMult(16, cp2);
    reorientCenterFB[cfb][1] = c.pack(cp2);
  }
}
