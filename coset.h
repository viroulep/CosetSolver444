#ifndef COSET_H
#define COSET_H

#include "cubepos.h"

const int COSET_N_COORD = 15582;
const int COSET_N_RAW_COORD = 735471;
const int COSET_N_SYM = 48;
const int COSET_SYM_SHIFT = 6;
const int COSET_SYM_MASK = ( 1 << COSET_SYM_SHIFT ) - 1;
const int COSET_N_MOVES = 36;

class coset {
    public :
    unsigned short center_rl_sym;
    unsigned char sym;
    
    static unsigned int sym2raw[COSET_N_COORD]; // Get a representative of a sym-coordinate
    static unsigned int raw2sym[COSET_N_RAW_COORD]; // Get the sym-coordinate of a raw coordinate
    static unsigned long long hasSym[COSET_N_COORD]; // Stores which positions are symmetric to which symmetries
    static unsigned int moveTable[COSET_N_COORD][COSET_N_MOVES]; // Move table of the sym-coordinate
    
    static unsigned char ptable[COSET_N_COORD>>1]; // Distance table of all coset positions

    /* Set one bit to 1 in the table at a certain index. */
    inline static void set1bit(unsigned char table[], int index) {
      table[index>>3] |= (unsigned char)( 1 << ( index & 0x7 ));
    }

    /* Get the state of one bit in the table at a certain index. */
    inline static unsigned char get1bit(unsigned char table[], int index) {
      return ( table[index>>3] >> ( index & 0x7 )) & 1;
    }

    inline static void writeTable (int index, int value) {
        ptable[index >> 1] ^= (0x0f ^ value) << ((index & 1) << 2);
    }

    inline static int readTable (int index) {
        return (ptable[index >> 1] >> ((index & 1) << 2)) & 0x0f;
    }

    inline bool isSolved() const{
        return center_rl_sym == 0;
    }

    void init();
    void unpack(cubepos &cube, unsigned int center_raw);
    unsigned int pack(const cubepos &cube);
    void initSym2Raw();
    void pack_all(const cubepos &cube);
    void initMove();
    void moveTo( int m, coset &c ) const;
    void fillPruningTable();

    inline int distance() const{
      return readTable(center_rl_sym);
    }
};

#endif
