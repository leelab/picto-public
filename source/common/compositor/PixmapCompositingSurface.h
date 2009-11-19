#ifndef _PIXMAPCOMPOSITINGSURFACE_H_
#define _PIXMAPCOMPOSITINGSURFACE_H_

#include "common.h"

#include "CompositingSurface.h"

#include <QPixmap>

namespace Picto {

class PixmapCompositingSurface : public CompositingSurface
{
public:
	PixmapCompositingSurface();

	void convertImage(QImage image);
	QString getTypeName();

private:
	QPixmap pixmap_;
};


}; //namespace Picto

#endif
