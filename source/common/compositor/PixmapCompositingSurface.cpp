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

QPixmap PixmapCompositingSurface::getPixmap()
{
	return pixmap_;
}

}; //namespace Picto
