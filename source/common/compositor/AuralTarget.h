#ifndef _AURALTARGET_H_
#define _AURALTARGET_H_

#include <QSharedPointer>

#include "../common.h"

#include "MixingSample.h"

namespace Picto {

/*!	\brief A target for audio.  
 *
 *	None of these have been implemented yet.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_CLASS AuralTarget
#else
class AuralTarget
#endif
{
public:
	AuralTarget();

	virtual QSharedPointer<MixingSample> generateMixingSample() = 0;
	virtual QString getTypeName() = 0;
	virtual void mix(QSharedPointer<MixingSample>) = 0;
	virtual void present() = 0;
};


}; //namespace Picto

#endif
