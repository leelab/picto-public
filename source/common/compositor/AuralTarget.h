#ifndef _AURALTARGET_H_
#define _AURALTARGET_H_

#include <QSharedPointer>

#include "../common.h"

#include "MixingSample.h"

namespace Picto {

/*!	\brief Objects of this class handle mixing of MixingSample sounds and playing over the computer's speakers. 
 * \details As described by a classic SAT-style analogy:
 *		VisualTarget : CompositingSurface :: AuralTarget : MixingSample
 *		and
 *		VisualElement : CompositingSurface :: AudioElement : MixingSample
 *	All sounds that need to be played are set from their owner AudioElement objects into MixingSample objects.  Every frame, these
 *  MixingSample objects are set into AuralTarget objects using mix().  Present() is called every frame, and this starts MixingSample objects
 *	that weren't yet playing but were mixed in, continues those that were playing and were mixed in, and stops those 
 *	that were playing and are no longer being mixed in.
 *	Each AuralTarget is responsible for generating MixingSample objects from generateMixingSample() of a type that it know's how
 *	to handle.
 *	\sa MixingSample
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS AuralTarget
#else
class AuralTarget
#endif
{
public:
	AuralTarget();
	/*!\brief Generates and returns a mixing sample of the correct type to be used with this AuralTarget.
	 * \details The returned MixingSample must be set up with audio date by the caller.
	 */
	virtual QSharedPointer<MixingSample> generateMixingSample() = 0;
	/*! \brief Returns the type name of this AuralTarget. 
	 */
	virtual QString getTypeName() = 0;
	/*! \brief Mixes the input MixingSample into this AuralTarget so that it will be audible this frame.
	 * \details Every MixingSample needs to be added to this AuralTarget during every frame that it needs to be audible. During the first frame when present() is called and a particular MixingSample was not added using mix(), that MixingSample will be stopped.
	 * \sa present()
	 */
	virtual void mix(QSharedPointer<MixingSample>) = 0;
	/*! \brief Causes all of the mixed in MixingSamples to correctly play back in this AuralTarget. Should be called every frame.
	 * \details During the first frame when present() is called and a particular MixingSample was not added using mix(), that MixingSample will be stopped.
	 * \sa mix()
	 */
	virtual void present() = 0;
};

}; //namespace Picto

#endif
