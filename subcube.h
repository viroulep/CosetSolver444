#include "cubepos.h"

const int N_COORD_CENTER_R = 24*23*22*21/(4*3*2*1);
const int N_COORD_CENTER_FB = 24*23*22*21*20*19*18*17/(8*7*6*5*4*3*2*1);
const int N_RAW_COORD_EDGES = 24*23*22*21*20*19*18*17*16*15*14*13/(12*11*10*9*8*7*6*5*4*3*2*1);
const int N_COORD_EDGES = 86048;

const int N_SYM = 16
const int SYM_SHIFT = 4
const int SYM_MASK = ( 1 << SYM_SHIFT ) - 1;
const int N_MOVES = 36;
const int N_MOVES_SUBGROUP = 28;

class coset {
    public :
    unsigned short center_r;
    unsigned int center_fb;
    unsigned int edge_raw;
    unsigned int edge_sym;
    unsigned char sym;
    
    static unsigned int sym2raw[N_COORD_EDGES]; // Get a representative of a sym-coordinate
    static unsigned short raw2sym[N_RAW_COORD_EDGES]; // Get the sym-coordinate of a raw coordinate
    static unsigned int hasSym[N_COORD_EDGES]; // Stores which positions are symmetric to which symmetries

    static unsigned short moveTableCenterR[N_COORD_CENTER_R][N_MOVES]; // Move table of the center R
    static unsigned short moveTableCenterFB[N_COORD_CENTER_FB][N_MOVES]; // Move table of the center FB
    static unsigned short moveTableEdge[N_COORD_EDGES][N_MOVES]; // Move table of the edge sym-coordinate

    static unsigned short conjTableCenterR[N_COORD_CENTER_R][N_MOVES]; // Conjugate table of the center R
    static unsigned short conjTableCenterFB[N_COORD_CENTER_FB][N_MOVES]; // Conjugate table of the center FB
    
    /* Set one bit to 1 in the table at a certain index. */
    inline static void set1bit(unsigned char table[], int index) {
      table[index>>>3] |= (unsigned char)( 1 << ( index & 0x7 ));
    }

    /* Get the state of one bit in the table at a certain index. */
    inline static unsigned char get1bit(unsigned char table[], int index) {
      return ( table[index>>>3] >>> ( index & 0x7 )) & 1;
    }


}