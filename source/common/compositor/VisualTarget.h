#ifndef _VISUALTARGET_H_
#define _VISUALTARGET_H_

#include "../common.h"

#include "CompositingSurface.h"

#include <QSharedPointer>

namespace Picto {

class VisualTarget
{
public:
	VisualTarget();

	virtual QSharedPointer<CompositingSurface> generateCompositingSurface() = 0;
	virtual QString getTypeName() = 0;
};


}; //namespace Picto

#endif
