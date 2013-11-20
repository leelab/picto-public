#ifndef _NULLCOMPOSITINGSURFACE_H_
#define _NULLCOMPOSITINGSURFACE_H_

#include "../common.h"

namespace Picto {

/*!	\brief A compositing surface that doesn't do anything
 *
 *	\details This would seem to be a total waste of time, but a null compositing surface
 *	can actually be really useful.  With a traditional compositing surface, 
 *	an experiment can only run as fast as we can render it.  With a null surface
 *	the engine can run an experiment as fast as the CPU allows.  This means that
 *	we can use a null compositing surface while analyzing an experiment.
 *
 *	\note Joey: When actually creating the Analysis system, we didn't actually use this,
 *	but it might be interesting to see if using this type of method could speed 
 *	things up when using RunToEnd in the PlaybackViewer.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class NullCompositingSurface
{
public:
	NullCompositingSurface();
};


}; //namespace Picto

#endif
