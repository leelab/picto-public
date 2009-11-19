#include "PixmapCompositingSurface.h"

namespace Picto {

PixmapCompositingSurface::PixmapCompositingSurface()
{
}

void PixmapCompositingSurface::convertImage(QImage image)
{
	pixmap_ = QPixmap::fromImage(image);
}

QString PixmapCompositingSurface::getTypeName()
{
	return QString("Pixmap");
}

}; //namespace Picto
