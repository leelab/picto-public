#ifndef _PIXMAPCOMPOSITINGSURFACE_H_
#define _PIXMAPCOMPOSITINGSURFACE_H_

#include "../common.h"

#include "CompositingSurface.h"

#include <QPixmap>

namespace Picto {

/*!	\brief A compositing surface using QPixmap
 *
 *	QPixmap is a basic bitmap impage.  It can be painted to, and is really easy
 *	to work with.  The downside is that it is really slow (not surprising for a
 *	software bitmap).  The PixmapCompositing surface would probably not be used
 *	in PictoDirector, but it is used in PictoWorkstation so that the user can
 *	see what is being displayed by a Director instance.
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
