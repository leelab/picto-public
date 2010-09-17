#ifndef _AURALTARGET_H_
#define _AURALTARGET_H_

#include "../common.h"

#include "MixingSample.h"

#include <QSharedPointer>

namespace Picto {

/*!	\brief A target for audio.  
 *
 *	None of these have been implemented yet.
 */
class AuralTarget
{
public:
	AuralTarget();

	virtual QSharedPointer<MixingSample> generateMixingSample() = 0;
};


}; //namespace Picto

#endif
