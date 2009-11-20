#ifndef _VISUALTARGET_H_
#define _VISUALTARGET_H_

#include "../common.h"

#include "CompositingSurface.h"

#include <QSharedPointer>

namespace Picto {

class VisualTarget
{
public:
	VisualTarget(bool _bWindowed = false, int _width = 800, int _height = 600);

	virtual QSharedPointer<CompositingSurface> generateCompositingSurface() = 0;
	virtual QString getTypeName() = 0;
	virtual void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface) = 0;
	virtual void present() = 0;
	virtual QRect getDimensions();

protected:
	bool bWindowed_;
	int width_;
	int height_;
};


}; //namespace Picto

#endif
