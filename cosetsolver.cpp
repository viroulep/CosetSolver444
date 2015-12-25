#include "cubepos.h"
#include "coset.h"
#include "subcube.h"
#include <iostream>
#include <set>
#include <cstring>

std::set<subcube> world;
int maxsearchdepth = 6;
long long uniq = 0;
long long probes = 0;

void slowsearch1(const coset &c, const subcube &sc, int togo, unsigned char syll) {
  if (togo == 0) {
    if (c.isSolved()) {
      probes++;
      subcube scs;
      scs = sc;
      //scs.print();
      bool conv = scs.convertToSC();
      scs.canonizeSC();
      world.insert(scs);
    }
    return;
  }
  togo--;

  coset c2;
  subcube sc2;

  //unsigned long long movemask = cubepos::mask_syll[syll];
  /* Fix that later... */
  unsigned long long movemask = (0x1ull << N_STAGE_MOVES) - 1;
  while (movemask) {
    int mv = ffsll(movemask) - 1;
    movemask &= movemask - 0x1ull;
    c.moveTo(mv, c2);
    int nd = c2.distance();
    if (nd <= togo) {
      sc.moveTo(mv, sc2);
      unsigned char new_syll = cubepos::next_syll[syll][mv];
      slowsearch1(c2, sc2, togo, new_syll);
    }
  }
}

void slowsearch1(const coset &c, const subcube &sc) {
  duration();
  for (int d=c.distance(); d < maxsearchdepth; d++) {
    probes = 0;
    long long prevlev = uniq;
    slowsearch1(c, sc, d, SYLL_NOMOVE);
    uniq = world.size();
    long long thislev = uniq - prevlev;
    std::cout << "Tests at " << d << " " << probes << " in " << duration() << " uniq " << uniq << " lev " << thislev << std::endl;
  }
}

int main(int argc, char *argv[]) {

	cubepos cp;
	cp.init();
	coset c;
	c.init();
	subcube sc;
	sc.init();

	cp.identity();
	c.pack_all(cp);
	sc.pack_all(cp);

	slowsearch1(c, sc);
	return 0;
}

