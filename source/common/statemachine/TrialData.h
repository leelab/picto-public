#ifndef _TRIALDATA_H_
#define _TRIALDATA_H_

#include "../common.h"


namespace Picto {

/*!	\brief Unimplemented.  Storage for information about a trial
 *
 *	This is unimplemented.  The original purpose of TrialData was to store information about 
 *	an individual trial (for example, the color of the fixation target, or the stage reached).
 *	This would be roughly equivalent to the trial blob in Orion.
 *
 *	I never implemented this because assuming you have the behavioral data stream, the 
 *	state machine, and possibly the state of the random number generator you should be
 *	able to regenerate the experiment in full, so there should be no additional data 
 *	that needs to be stored.
 */
class TrialData
{
public:
	TrialData();
};


}; //namespace Picto

#endif
