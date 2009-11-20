#ifndef _PIXMAPVISUALTARGET_H_
#define _PIXMAPVISUALTARGET_H_

#include "../common.h"

#include "VisualTarget.h"

#include <QPixmap>
#include <QWidget>

namespace Picto {

struct PICTOLIB_CLASS PixmapVisualTarget : public VisualTarget, public QWidget
{
public:
	PixmapVisualTarget(bool _bWindowed = false, int _width = 800, int _height = 600);

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();
	void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface);
	void present();

	void drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text);

protected:
	void paintEvent(QPaintEvent *);

private:
	unsigned int surfaceActingAsBackBuffer_;
	std::vector<QPixmap> pixmapCompositingSurfaces_;
};

}; //namespace Picto

#endif
