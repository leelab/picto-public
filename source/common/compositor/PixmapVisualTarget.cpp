#include "PixmapVisualTarget.h"

#include "PixmapCompositingSurface.h"

namespace Picto {

PixmapVisualTarget::PixmapVisualTarget()
{
}

QSharedPointer<CompositingSurface> PixmapVisualTarget::generateCompositingSurface()
{
	QSharedPointer<PixmapCompositingSurface> pixmapCompositingSurface(new PixmapCompositingSurface());

	return pixmapCompositingSurface;
}

QString PixmapVisualTarget::getTypeName()
{
	return QString("Pixmap");
}

}; //namespace Picto
