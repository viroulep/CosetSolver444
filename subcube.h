#ifndef SUBCUBE_H
#define SUBCUBE_H

#include "cubepos.h"

const int SUBCUBE_N_COORD_CENTER_R = 10626;
const int SUBCUBE_N_COORD_CENTER_FB = 735471;
const int SUBCUBE_N_RAW_COORD_EDGES = 1352078;
const int SUBCUBE_N_COORD_EDGES = 86048;

const int SUBCUBE_N_SYM = 16;
const int SUBCUBE_SYM_SHIFT = 4;
const int SUBCUBE_SYM_MASK = ( 1 << SUBCUBE_SYM_SHIFT ) - 1;
const int SUBCUBE_N_MOVES_ALL = 36;
const int SUBCUBE_N_MOVES = 28;

const int SUBCUBE_MIN_CENTER_R_SC = 4844; // Determined programmatically
const int SUBCUBE_N_COORD_CENTER_R_SC = 35;
const int SUBCUBE_N_COORD_CENTER_FB_SC = 12870;


/* The subcube class holds the information of the first two stages of Chen's 4x4x4 solver
 * It consists of three coordinates:
 * - a center_r coordinate, which stores the location of the four centers from the right face among all centers (C_4^8 = 10626 different positions)
 * - a center_fb coordinate, which stores the location of the eight centers from the front and back faces among all centers (C_8^24 = 735471 positions)
 * - an edge coordinate, which stores the location of high edges or low edges among all edges (C_12^24/2 = 1352078 positions)
 *   this coordinate is reduced by symmetry, giving 86048 unique edge positions
 *
 * This class is also optimised to operate inside the subgroup H generated by <U, u2, D, d2, R, r, L, l, F, f2, B, b2>
 * In this case, the coordinates are further reduced:
 * - the center_r coordinate can have C_4^8 = 70 different positions inside H (position of R-centers inside the RL faces),
 *   but we don't need to hold if we are storing the position of R- or L-centers, so it can be reduced by a factor of 2.
 *   So the previous center_r coordinate is transformed to the new coordinate using a lookup table
 * - the center_fb coordinate can have C_8^16 = 12870 different positions inside H.
 *   We don't need to do any transformation from the old center_fb to the new center_fb coordinate
 * - the edge coordinate does not change whether we operate inside H or not, so this coordinate is unchanged.
 * We have to take care of the reorientation when converting to a position inside H,
 * because the first phase can solve RL-centers on RL faces, but also on FB faces or UD faces.
 * So we need to rotate all coordinates so that RL-centers are placed on RL faces.
 */
class subcube {
    public :
    unsigned short center_r;
    unsigned int center_fb;
    unsigned int edge_sym;
    unsigned char sym;
    
    static unsigned int sym2raw[SUBCUBE_N_COORD_EDGES]; // Get a representative of a sym-coordinate
    static unsigned int raw2sym[SUBCUBE_N_RAW_COORD_EDGES]; // Get the sym-coordinate of a raw coordinate
    static unsigned int hasSym[SUBCUBE_N_COORD_EDGES]; // Stores which positions are symmetric to which symmetries

    static unsigned short moveTableCenterR[SUBCUBE_N_COORD_CENTER_R][SUBCUBE_N_MOVES_ALL]; // Move table of the center R
    static unsigned int moveTableCenterFB[SUBCUBE_N_COORD_CENTER_FB][SUBCUBE_N_MOVES_ALL]; // Move table of the center FB
    static unsigned int moveTableEdge[SUBCUBE_N_COORD_EDGES][SUBCUBE_N_MOVES_ALL]; // Move table of the edge sym-coordinate

    static unsigned short conjTableCenterR[SUBCUBE_N_COORD_CENTER_R][SUBCUBE_N_MOVES_ALL]; // Conjugate table of the center R
    static unsigned int conjTableCenterFB[SUBCUBE_N_COORD_CENTER_FB][SUBCUBE_N_MOVES_ALL]; // Conjugate table of the center FB
   
    static unsigned char centerRToSC[SUBCUBE_N_COORD_CENTER_R-SUBCUBE_MIN_CENTER_R_SC]; // Convert a full cube center r to subcube center r coordinate
    static unsigned char moveTableCenterRSC[SUBCUBE_N_COORD_CENTER_R_SC][SUBCUBE_N_MOVES]; // Move table of the subcube center R
    static unsigned char conjTableCenterRSC[SUBCUBE_N_COORD_CENTER_R_SC][SUBCUBE_N_MOVES]; // Conjugate table of the subcube center R

    static unsigned int reorientEdge[SUBCUBE_N_COORD_EDGES][2]; // Reorient edges when putting RL centers in RL faces
    static unsigned short reorientCenterR[SUBCUBE_N_COORD_CENTER_R][2]; // Reorient center_r when putting RL centers in RL faces
    static unsigned int reorientCenterFB[SUBCUBE_N_COORD_CENTER_FB][2]; // Reorient center_fb when putting RL centers in RL faces
 
    /* Set one bit to 1 in the table at a certain index. */
    inline static void set1bit(unsigned char table[], int index) {
      table[index>>3] |= (unsigned char)( 1 << ( index & 0x7 ));
    }

    /* Get the state of one bit in the table at a certain index. */
    inline static unsigned char get1bit(unsigned char table[], int index) {
      return ( table[index>>3] >> ( index & 0x7 )) & 1;
    }

    void init();
    void unpack_center_r(cubepos &cube, unsigned short center_r);
    unsigned short pack_center_r(const cubepos &cube);
    void unpack_center_fb(cubepos &cube, unsigned int center_fb);
    unsigned int pack_center_fb(const cubepos &cube);
    void unpack_center_r_sc(cubepos &cube, unsigned char center_r);
    unsigned char pack_center_r_sc(const cubepos &cube);
    void unpack_edge(cubepos &cube, unsigned int edge_raw);
    unsigned int pack_edge(const cubepos &cube);
    void initSym2Raw ();
    void pack_all(const cubepos &cube);
    void initMove ();
    void moveTo( int m, subcube &c );
    void canonize();
    void initToSC();
    void initMoveSC();
    void initReorientSC();
    bool convertToSC();
    void moveToSC( int m, subcube &c );
    void canonizeSC();
    void print();
};

#endif
