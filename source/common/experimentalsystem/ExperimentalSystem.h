#ifndef _EXPERIMENTALSYSTEM_H_
#define _EXPERIMENTALSYSTEM_H_

#include "../common.h"

namespace Picto {

/*! \brief Unimplemented.  Stores information about the Experimental system being used
 *
 *	It makes sense to record the experimental setup being used for each experiment.
 *	This class will eventually provide that capability.  I am envisioning it as a container
 *	that stores all of the other equipment classes (Audio, Behvioral, etc).  Ideally, these
 *	classes will all collect their setup info in a completely automated fashion.  However,
 *	it seems likely that we will require input from the user for some of them (e.g. it is
 *	unlikely that we will be able to determine the subject's position without asking the user)
 *	We will need to come up with some sort of scheme that isn't too intrusive (no one wants
 *	to fill out 20 pages of junk efor each experiment), but also that actually gets up to date
 *	info (maybe require the user to update once a week?)
 */
class ExperimentalSystem
{
public:
	ExperimentalSystem();
};


}; //namespace Picto

#endif
