#include "mtrand.h"
#include "random/MersenneTwister.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief A test function for making sure that the random number seed system works.
 *	\details This should probably be removed.  It looks like the debug version of the workstation is calling
 *	it every time it starts up.  This seems like a rediculous waste of time.
 */
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
