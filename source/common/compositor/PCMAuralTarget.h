#ifndef _PCMAURALTARGET_H_
#define _PCMAURALTARGET_H_


#include <QSharedPointer>
#include <QHash>

#include "../common.h"
#include "AuralTarget.h"
#include "MixingSample.h"

namespace Picto {

/*! \brief A pulse code modulation (PCM) aural target
 *
 *	This hasn't been implemented yet, and I'm not sure how it was going
 *	to be implemented...  - Matt Gay 8/31/2010
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
	QList<QSharedPointer<MixingSample>> soundsToMix_;
	QSharedPointer<QHash<MixingSample*,QSharedPointer<MixingSample>>> activeSoundLookup_;
};


}; //namespace Picto

#endif
