#ifndef _PIXMAPVISUALTARGET_H_
#define _PIXMAPVISUALTARGET_H_

#include "../common.h"

#include "VisualTarget.h"

#include <QPixmap>

namespace Picto {

struct PICTOLIB_CLASS PixmapVisualTarget : public VisualTarget
{
public:
	PixmapVisualTarget();

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();

private:
	int surfaceActingAsBackBuffer;
	std::vector<QPixmap> pixmapCompositingSurfaces;
	QPixmap pixmapSurface1_;
	QPixmap pixmapSurface2_;
};

}; //namespace Picto

#endif
