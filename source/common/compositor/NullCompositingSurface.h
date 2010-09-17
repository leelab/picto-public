#ifndef _NULLCOMPOSITINGSURFACE_H_
#define _NULLCOMPOSITINGSURFACE_H_

#include "../common.h"

namespace Picto {

/*!	\brief A compositing surface that doesn't do anything
 *
 *	This would seem to be a total waste of time, but a null compositing surface
 *	will actually be really useful.  With a traditional compositing surface, 
 *	an experiment can only run as fast as we can render it.  With a null surface
 *	the engine can run an experiment as fast as the CPU allows.  This means that
 *	we can use a null compositing surface while analyzing an experiment.
 *
 *	This class is not yet implemented, although implementation should consist of 
 *	nothing more than doing nothing when convertImage is called.
 */
class NullCompositingSurface
{
public:
	NullCompositingSurface();
};


}; //namespace Picto

#endif
