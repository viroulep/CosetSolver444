#include "cubepos.h"

const int N_COORD = 15582;
const int N_RAW_COORD = 735471;
const int N_SYM = 48
const int SYM_SHIFT = 6
const int SYM_MASK = ( 1 << SYM_SHIFT ) - 1;
const int N_MOVES = 36;

class coset {
	public :
		unsigned int center_rl_raw;
		unsigned short center_rl_sym;
    unsigned char sym;
    
    static unsigned int sym2raw[N_COORD]; // Get a representative of a sym-coordinate
    static unsigned short raw2sym[N_RAW_COORD]; // Get the sym-coordinate of a raw coordinate
    static unsigned long long hasSym[N_COORD]; // Stores which positions are symmetric to which symmetries
    static unsigned short moveTable[N_COORD][N_MOVES]; // Move table of the sym-coordinate
    
    /* Set one bit to 1 in the table at a certain index. */
    inline static void set1bit(unsigned char table[], int index) {
      table[index>>>3] |= (unsigned char)( 1 << ( index & 0x7 ));
    }

    /* Get the state of one bit in the table at a certain index. */
    inline static unsigned char get1bit(unsigned char table[], int index) {
      return ( table[index>>>3] >>> ( index & 0x7 )) & 1;
    }


}