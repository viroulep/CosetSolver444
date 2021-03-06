#include <iostream>
#include "subcube.h"

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
  static int initialized = 0;
  if (initialized)
    return;
  initialized = 1;

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

/* Unpack a raw centerFB coord to a cube */
void subcube::unpack_center_fb(cubepos &cube, unsigned int center_raw)
{
  unsigned char udrl = 0;
  unsigned char fb = 8;
  int r = 8;
  for (int i = 23; i >= 0; i--) {
    if (center_raw < cubepos::Cnk[i][r] ) {
      cube.centers[i] = udrl++/4;
      if (udrl == 8) udrl = 16;
    } else {
      center_raw -= cubepos::Cnk[i][r--];
      cube.centers[i] = fb++/4;
    }
  }
}

/* Pack a cube into the raw centerFB coord */
unsigned int subcube::pack_center_fb(const cubepos &cube){
  unsigned int center_raw = 0;
  int r = 8;
  for (int i = 23; i >= 0; i--) {
    if ((cube.centers[i] / 2) == 1 ) {
      center_raw += cubepos::Cnk[i][r--];
    }
  }
  return center_raw;
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
  unsigned int repIdx = 0;
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
      if (raw_coord >= SUBCUBE_N_RAW_COORD_EDGES) std::cout << "subcube::initSym2Raw: error edge raw coord too big!" << std::endl;
      set1bit( isRepTable, raw_coord );
      raw2sym[raw_coord] = ( repIdx << SUBCUBE_SYM_SHIFT ) + cubepos::invSymIdx[s];
      if( raw_coord == u )
        hasSym[repIdx] |= (1 << s);
    }
    if (repIdx >= SUBCUBE_N_COORD_EDGES) {
      std::cout << "subcube::initSym2Raw: error sym coordinate too big!" << std::endl;
      break;
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
  center_fb = pack_center_fb(cube);
  unsigned int edge_raw = pack_edge(cube);
  edge_sym = raw2sym[edge_raw];
  sym = edge_sym & SUBCUBE_SYM_MASK;
  edge_sym >>= SUBCUBE_SYM_SHIFT;
}

/* Initialise all move and conjugate arrays */
void subcube::initMove (){
  cubepos cube1;
  cubepos cube2;

  /* Initialise edges move array */  
  for (int u = 0; u < SUBCUBE_N_COORD_EDGES; u++) {
    unpack_edge(cube1, sym2raw[u]);
    for (int m = 0; m < SUBCUBE_N_MOVES_ALL; m++) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTableEdge[u][m] = raw2sym[pack_edge(cube2)];
    }
  }

  /* Initialise centerR move and conj arrays */
  for (int u = 0; u < SUBCUBE_N_COORD_CENTER_R; u++) {
    unpack_center_r(cube1, u);
    for (int m = 0; m < SUBCUBE_N_MOVES_ALL; m++) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTableCenterR[u][m] = pack_center_r(cube2);
    }
    for (int s = 0; s < SUBCUBE_N_SYM; s++) {
      //cube1.conjugate (s, cube2);
      cube1.rightMult (cubepos::invSymIdx[s], cube2);
      conjTableCenterR[u][s] = pack_center_r(cube2);
    }
  }

  /* Initialise centerFB move and conj arrays */
  for (int u = 0; u < SUBCUBE_N_COORD_CENTER_FB; u++) {
    unpack_center_fb(cube1, u);
    for (int m = 0; m < SUBCUBE_N_MOVES_ALL; m++) {
      cube2 = cube1;
      cube2.move (cubepos::stage2moves[m]);
      moveTableCenterFB[u][m] = pack_center_fb(cube2);
    }
    for (int s = 0; s < SUBCUBE_N_SYM; s++) {
      //cube1.conjugate (s, cube2);
      cube1.rightMult (cubepos::invSymIdx[s], cube2);
      conjTableCenterFB[u][s] = pack_center_fb(cube2);
    }
  }
}

/* Apply a move to the subgroup state */
void subcube::moveTo( int m, subcube &c ) const{
  c.center_r = moveTableCenterR[center_r][m];
  c.center_fb = moveTableCenterFB[center_fb][m];
  c.edge_sym = moveTableEdge[edge_sym][cubepos::moveConjugateStage[m][sym]];
  c.sym = cubepos::symIdxMultiply[c.edge_sym & SUBCUBE_SYM_MASK][sym];
  c.edge_sym >>= SUBCUBE_SYM_SHIFT;
}

/* Get the canon state without any symmetry applied */
void subcube::canonize(){
  center_r = conjTableCenterR[center_r][sym];
  center_fb = conjTableCenterFB[center_fb][sym];
  sym = 0;

  /* We may have symmetric position for edges, so that there are multiple 
   * (edge_sym, sym) pairs that correspond to the same position.
   * To get a canonical state, we iterate through all the symmetric positions
   * and return the minimum of the two other coordinates in lexicographic order
   */
  unsigned int allsyms = hasSym[edge_sym];
  if (allsyms == 0) return; // No symmetry in edges position

  unsigned short min_center_r = center_r;
  unsigned int min_center_fb = center_fb;
  for (int s=0; allsyms!=0; allsyms>>=1, s++) {
    if ((allsyms & 1) == 0) continue;
    unsigned short new_center_r = conjTableCenterR[center_r][s];
    if (new_center_r > min_center_r) continue;
    unsigned int new_center_fb = conjTableCenterFB[center_fb][s];
    if ((new_center_r < min_center_r) || (new_center_fb < min_center_fb)) {
      min_center_r = new_center_r;
      min_center_fb = new_center_fb;
    } 
  }
  center_r = min_center_r;
  center_fb = min_center_fb;
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
      //cube1.conjugate (s, cube2);
      cube1.rightMult (cubepos::invSymIdx[s], cube2);
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
    cp1.rightMult(32, cp2);
    reorientEdge[esym][1] = raw2sym[pack_edge(cp2)];
  }

  for (int cr=0; cr<SUBCUBE_N_COORD_CENTER_R; cr++){
    unpack_center_r(cp1, cr);
    cp1.rightMult(16, cp2);
    reorientCenterR[cr][0] = pack_center_r(cp2);
    cp1.rightMult(32, cp2);
    reorientCenterR[cr][1] = pack_center_r(cp2);
  }

  for (int cfb=0; cfb<SUBCUBE_N_COORD_CENTER_FB; cfb++){
    unpack_center_fb(cp1, cfb);
    cp1.rightMult(16, cp2);
    reorientCenterFB[cfb][0] = pack_center_fb(cp2);
    cp1.rightMult(32, cp2);
    reorientCenterFB[cfb][1] = pack_center_fb(cp2);
  }
}

/* Convert the cube coordinates to subcube coordinates, and return if it is a success */
bool subcube::convertToSC(){
	/* Reorienting the cube, using center_r to guess the right orientation. */
	int reorient = -1;

	int new_center_r = center_r;
	while ((reorient < 2) && ((new_center_r < SUBCUBE_MIN_CENTER_R_SC) || (centerRToSC[new_center_r-SUBCUBE_MIN_CENTER_R_SC] == 0xff))) {
		reorient++;
		if (reorient < 2)
			new_center_r = reorientCenterR[center_r][reorient];
	}

	if (reorient == 2){
		std::cout << "subcube: error when converting to SC, center_r not in SC" << std::endl;
		return false;
	}
	
	if (reorient >= 0){
		center_r = new_center_r;
		center_fb = reorientCenterFB[center_fb][reorient];
		edge_sym = reorientEdge[edge_sym][reorient];
		sym = cubepos::symIdxMultiply[edge_sym & SUBCUBE_SYM_MASK][sym];
		edge_sym >>= SUBCUBE_SYM_SHIFT;
	}

	/* Check the center_fb coordinate */
	if (center_fb >= SUBCUBE_N_COORD_CENTER_FB_SC){
		std::cout << "subcube: error when converting to SC, center_fb not in SC" << std::endl;
		return false;
	}

	/* Transform the center_r coordinate */
	center_r = centerRToSC[center_r-SUBCUBE_MIN_CENTER_R_SC];
	return true;
}

/* Apply a move to the subgroup state inside H */
void subcube::moveToSC( int m, subcube &c ){
  c.center_r = moveTableCenterRSC[center_r][m];
  c.center_fb = moveTableCenterFB[center_fb][m];
  c.edge_sym = moveTableEdge[edge_sym][cubepos::moveConjugateStage[m][sym]];
  c.sym = cubepos::symIdxMultiply[c.edge_sym & SUBCUBE_SYM_MASK][sym];
  c.edge_sym >>= SUBCUBE_SYM_SHIFT;
}

/* Get the canon state without any symmetry applied in H */
void subcube::canonizeSC(){
  center_r = conjTableCenterRSC[center_r][sym];
  center_fb = conjTableCenterFB[center_fb][sym];
  sym = 0;

  /* We may have symmetric position for edges, so that there are multiple 
   * (edge_sym, sym) pairs that correspond to the same position.
   * To get a canonical state, we iterate through all the symmetric positions
   * and return the minimum of the two other coordinates in lexicographic order
   */
  unsigned int allsyms = hasSym[edge_sym];
  if (allsyms == 0) return; // No symmetry in edges position

  unsigned short min_center_r = center_r;
  unsigned int min_center_fb = center_fb;
  for (int s=0; allsyms!=0; allsyms>>=1, s++) {
    if ((allsyms & 1) == 0) continue;
    unsigned short new_center_r = conjTableCenterRSC[center_r][s];
    if (new_center_r > min_center_r) continue;
    unsigned int new_center_fb = conjTableCenterFB[center_fb][s];
    if ((new_center_r < min_center_r) || (new_center_fb < min_center_fb)) {
      min_center_r = new_center_r;
      min_center_fb = new_center_fb;
    } 
  }
  center_r = min_center_r;
  center_fb = min_center_fb;
}

void subcube::print(){
  std::cout << "es: " << edge_sym << " s: " << (int)sym << " cr: " << center_r << " cfb: " << center_fb << std::endl;
}

