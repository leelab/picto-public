#ifndef _PIXMAPVISUALTARGET_H_
#define _PIXMAPVISUALTARGET_H_

#include <QPixmap>

#include "../common.h"

#include "VisualTarget.h"

namespace Picto {

/*!	\brief The visual target used with a pixmap compositing surface.
 *
 *	This visual target works only with a PixmapCompositingSurface.  Like all 
 *	VisualTargets, it is responsible for drawing to the screen.  Since Pixmaps are
 *	so easy to work with, this is a really simple target.  It holds 2 pixmap
 *	compositing surface, and flips back and forth between them (so that we never
 *	draw directly to the screen).
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
struct PICTOLIB_API PixmapVisualTarget : public VisualTarget
#else
struct PixmapVisualTarget : public VisualTarget
#endif
{
	Q_OBJECT
public:
	PixmapVisualTarget(bool _bWindowed = false, int _width = 800, int _height = 600);
	virtual ~PixmapVisualTarget();

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();
	void draw(QPoint location, QPoint compositingSurfaceOffset, QSharedPointer<CompositingSurface> compositingSurface);
	void present();
	void clear();
	/*! \copydoc VisualTarget::latestFrameSuccesfullyRendered
	 *	\details In the case of PixmapVisualTarget, QPixmaps are simple enough that we are just always returning true.
	 */
	bool latestFrameSuccesfullyRendered(){return true;};

	void drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text);
	virtual QPoint viewportPointToTargetPoint(QPoint viewportPoint);
	virtual QPoint targetPointToViewportPoint(QPoint targetPoint);

protected:
	void paint(QPaintDevice *widget);

private:
	unsigned int surfaceActingAsBackBuffer_;
	std::vector<QPixmap> pixmapCompositingSurfaces_;
	float frameSynchedZoom_;
};

}; //namespace Picto

#endif
