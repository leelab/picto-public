#ifndef _PCMAURALTARGET_H_
#define _PCMAURALTARGET_H_


#include <QSharedPointer>
#include <QHash>

#include "../common.h"
#include "AuralTarget.h"
#include "MixingSample.h"

namespace Picto {

/*! \brief A pulse code modulation (PCM) aural target
 * \details The original concept for this AuralTarget seems to be that all audio inputs would be converted
 * into pulse code modulation, after which these samples would be mixed into one big pulse code modulated
 * audio sample which would be played in this PCMAuralTarget.  In practice, things didn't work out exactly
 * in that way.
 * Currently the way this works is a mixing sample is generated using generateMixingSample().  It is then
 * initialized by the caller with audio data. Every frame during which this MixingSample should be audible,
 * it must be mixed into the PCMAuralTarget by calling mix().  Instead of updating some big 
 * PCM object as one might expect, we simply add the input MixingSample to an internal soundsToMix_ list.
 * Then, every frame present() is called.  In present(), all MixingSamples that were mixed in using mix() are
 * either started or allowed to continue playing over the computer's speakers and any mixing samples that
 * were mixed in last frame that were not mixed in this frame are set to stop.
 * \author Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS PCMAuralTarget : public AuralTarget
#else
class PCMAuralTarget : public AuralTarget
#endif
{
public:
	PCMAuralTarget();

	virtual QSharedPointer<MixingSample> generateMixingSample();
	virtual QString getTypeName();
	virtual void mix(QSharedPointer<MixingSample>);
	virtual void present();
private:
	QList<QSharedPointer<MixingSample>> soundsToMix_;	//!< Contains all sounds that should be mixed into the audio channel this frame
	QSharedPointer<QHash<MixingSample*,QSharedPointer<MixingSample>>> activeSoundLookup_; //!<Contains all sounds that were playing last frame
};


}; //namespace Picto

#endif
