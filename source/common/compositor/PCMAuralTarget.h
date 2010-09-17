#ifndef _PCMAURALTARGET_H_
#define _PCMAURALTARGET_H_

#include "../common.h"

#include "AuralTarget.h"
#include "MixingSample.h"
#include "PCMMixingSample.h"

#include <QSharedPointer>

namespace Picto {

/*! \brief A pulse code modulation (PCM) aural target
 *
 *	This hasn't been implemented yet, and I'm not sure how it was going
 *	to be implemented...  - Matt Gay 8/31/2010
 */
struct PICTOLIB_CLASS PCMAuralTarget : public AuralTarget
{
public:
	PCMAuralTarget();

	QSharedPointer<MixingSample> generateMixingSample();
};


}; //namespace Picto

#endif
