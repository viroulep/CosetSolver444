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

class subcube {
    public :
    unsigned short center_r;
    unsigned int center_fb;
    unsigned int edge_raw;
    unsigned int edge_sym;
    unsigned char sym;
    
    static unsigned int sym2raw[SUBCUBE_N_COORD_EDGES]; // Get a representative of a sym-coordinate
    static unsigned short raw2sym[SUBCUBE_N_RAW_COORD_EDGES]; // Get the sym-coordinate of a raw coordinate
    static unsigned int hasSym[SUBCUBE_N_COORD_EDGES]; // Stores which positions are symmetric to which symmetries

    static unsigned short moveTableCenterR[SUBCUBE_N_COORD_CENTER_R][SUBCUBE_N_MOVES_ALL]; // Move table of the center R
    static unsigned short moveTableCenterFB[SUBCUBE_N_COORD_CENTER_FB][SUBCUBE_N_MOVES_ALL]; // Move table of the center FB
    static unsigned short moveTableEdge[SUBCUBE_N_COORD_EDGES][SUBCUBE_N_MOVES_ALL]; // Move table of the edge sym-coordinate

    static unsigned short conjTableCenterR[SUBCUBE_N_COORD_CENTER_R][SUBCUBE_N_MOVES_ALL]; // Conjugate table of the center R
    static unsigned short conjTableCenterFB[SUBCUBE_N_COORD_CENTER_FB][SUBCUBE_N_MOVES_ALL]; // Conjugate table of the center FB
    
    /* Set one bit to 1 in the table at a certain index. */
    inline static void set1bit(unsigned char table[], int index) {
      table[index>>3] |= (unsigned char)( 1 << ( index & 0x7 ));
    }

    /* Get the state of one bit in the table at a certain index. */
    inline static unsigned char get1bit(unsigned char table[], int index) {
      return ( table[index>>3] >> ( index & 0x7 )) & 1;
    }

    void init();
    void unpack_center_r(cubepos cube, unsigned short center_r);
    unsigned short pack_center_r(cubepos cube);
    void unpack_edge(cubepos cube, unsigned int edge_raw);
    unsigned int pack_edge(cubepos cube);
    void initSym2Raw ();
    void pack_all(cubepos cube);
    void initMove ();
    void moveTo( int m, subcube c );
    void canonize( int sym );

};

#endif