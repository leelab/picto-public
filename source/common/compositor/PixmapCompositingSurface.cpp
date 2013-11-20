#include "PixmapCompositingSurface.h"
#include "../memleakdetect.h"

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

/*! \brief Returns the pixmap that was created in this CompositingSurface based on the QImage input in convertImage
 */
QPixmap PixmapCompositingSurface::getPixmap()
{
	return pixmap_;
}

}; //namespace Picto
