#ifndef _PIXMAPCOMPOSITINGSURFACE_H_
#define _PIXMAPCOMPOSITINGSURFACE_H_

#include <QPixmap>

#include "../common.h"

#include "CompositingSurface.h"

namespace Picto {

/*!	\brief A compositing surface based on an underlying QPixmap data structure
 *
 *	QPixmap is a basic bitmap impage.  It can be painted to, and is really easy
 *	to work with.  The downside is that it is really slow (not surprising for a
 *	software bitmap).  The PixmapCompositingSurface would probably not be used
 *	in PictoDirector, but it is multiplatform compatible and therefore used in 
 *	the PictoWorkstation so that the user can see what is being displayed by a Director instance.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PixmapCompositingSurface : public CompositingSurface
{
public:
	PixmapCompositingSurface();

	void convertImage(QImage image);
	QString getTypeName();

	QPixmap getPixmap();

private:
	QPixmap pixmap_;
};


}; //namespace Picto

#endif
