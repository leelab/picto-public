#ifndef _AURALTARGET_H_
#define _AURALTARGET_H_

#include "../common.h"

#include "MixingSample.h"

#include <QSharedPointer>

namespace Picto {

class AuralTarget
{
public:
	AuralTarget();

	virtual QSharedPointer<MixingSample> generateMixingSample() = 0;
};


}; //namespace Picto

#endif
