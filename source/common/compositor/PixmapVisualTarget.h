#ifndef _PIXMAPVISUALTARGET_H_
#define _PIXMAPVISUALTARGET_H_

#include "../common.h"

#include "VisualTarget.h"

#include <QPixmap>

namespace Picto {

#if defined WIN32 || defined WINCE
struct PICTOLIB_API PixmapVisualTarget : public VisualTarget
#else
struct PixmapVisualTarget : public VisualTarget
#endif
{
public:
	PixmapVisualTarget(bool _bWindowed = false, int _width = 800, int _height = 600);
	~PixmapVisualTarget();

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();
	void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface);
	void present();
	void clear();

	void drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text);

protected:
	void paint(QPaintDevice *widget);

private:
	unsigned int surfaceActingAsBackBuffer_;
	std::vector<QPixmap> pixmapCompositingSurfaces_;
};

}; //namespace Picto

#endif
