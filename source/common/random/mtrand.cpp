#include "mtrand.h"
#include "random/MersenneTwister.h"
#include "../memleakdetect.h"

namespace Picto {

bool testMTRand()
{
	MTRand rng;

    rng.seed(1);
    for (int i=0; i<10000; i++)
        rng.randInt();
    if (rng.randInt()!=2915232614UL)
		return(false);

	return(true);
}

}; //namespace Picto
