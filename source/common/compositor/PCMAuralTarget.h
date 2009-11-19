#ifndef _PCMAURALTARGET_H_
#define _PCMAURALTARGET_H_

#include "../common.h"

#include "AuralTarget.h"
#include "MixingSample.h"
#include "PCMMixingSample.h"

#include <QSharedPointer>

namespace Picto {

struct PICTOLIB_CLASS PCMAuralTarget : public AuralTarget
{
public:
	PCMAuralTarget();

	QSharedPointer<MixingSample> generateMixingSample();
};


}; //namespace Picto

#endif
