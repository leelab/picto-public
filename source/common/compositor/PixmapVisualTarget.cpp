#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

#include "PixmapVisualTarget.h"
#include "PixmapCompositingSurface.h"
#include "../memleakdetect.h"

namespace Picto {

PixmapVisualTarget::PixmapVisualTarget(bool _bWindowed, int _width, int _height) :
	VisualTarget(_bWindowed, _width, _height)
{
	if(!bWindowed_)
	{
		//If we don't put in the line below.  All mouse values will be
		//about 25 pixels off of their screen locations since Qt will
		//take the invisible window frame into account when computing
		//mouse position.
		QWidget::setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

		QRect screenRect = QApplication::desktop()->screenGeometry(-1);
		width_ = screenRect.width();
		height_ = screenRect.height();
		resize(width_,height_);
		move(0,0);
	}
	else
	{
		//setMaximumSize(width_,height_);
		setFixedSize(width_,height_);
		resize(width_, height_);  //This might be redundant...
	}
	setAttribute(Qt::WA_NativeWindow,true);
	setAttribute(Qt::WA_PaintOnScreen,true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);

	setAutoFillBackground(true);

	pixmapCompositingSurfaces_.push_back(QPixmap(width(),height()));
	pixmapCompositingSurfaces_.push_back(QPixmap(width(),height()));

	pixmapCompositingSurfaces_[0].fill(QColor(0,0,0,0));
	pixmapCompositingSurfaces_[1].fill(QColor(0,0,0,0));

	surfaceActingAsBackBuffer_=1;
	frameSynchedZoom_ = zoom_;
}

PixmapVisualTarget::~PixmapVisualTarget()
{
	printf("PixmapVisualTarget detructor called!\n");
}

void PixmapVisualTarget::paint(QPaintDevice *widget)
{
	//Note that we aren't assuming that we're drawing on ourselves.
	QPainter painter(widget);
	painter.drawPixmap(QPoint(0,0),pixmapCompositingSurfaces_[~surfaceActingAsBackBuffer_ & 1]);
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

void PixmapVisualTarget::draw(QPoint location, QPoint compositingSurfaceOffset, QSharedPointer<CompositingSurface> compositingSurface)
{
	frameSynchedZoom_ = zoom_;
	Q_ASSERT(frameSynchedZoom_ > 0);
	if(compositingSurface->getTypeName()=="Pixmap")
	{
		QPainter painter(&pixmapCompositingSurfaces_[surfaceActingAsBackBuffer_]);
		painter.setRenderHint(QPainter::Antialiasing); 
		painter.setRenderHint(QPainter::TextAntialiasing);
		if(compositingSurface->scalable())
		{
			location = location-compositingSurfaceOffset;
			painter.setViewport((width_-(width_*frameSynchedZoom_))/2.0,(height_-(height_*frameSynchedZoom_))/2.0,width_*frameSynchedZoom_,height_*frameSynchedZoom_);
		}
		else
		{	
			location = targetPointToViewportPoint(location)-compositingSurfaceOffset;
		}

		painter.drawPixmap(location, compositingSurface.staticCast<PixmapCompositingSurface>()->getPixmap());
	}
}

void PixmapVisualTarget::present()
{
	surfaceActingAsBackBuffer_ = ~surfaceActingAsBackBuffer_ & 1;

	pixmapCompositingSurfaces_[surfaceActingAsBackBuffer_].fill(QColor(0,0,0,0));

	//Set first phosphor time !! THIS IS NOT ACCURATE YET
	setFirstPhosphorTime();
	repaint();
}

void PixmapVisualTarget::drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text)
{
	QPainter painter(&pixmapCompositingSurfaces_[surfaceActingAsBackBuffer_]);

	painter.setPen(color);
	painter.setFont(font);
	painter.drawText(rect, alignment, text);
}

QPoint PixmapVisualTarget::viewportPointToTargetPoint(QPoint viewportPoint)
{
	Q_ASSERT(frameSynchedZoom_ > 0);
	float x = (viewportPoint.x()-(width_-(width_*frameSynchedZoom_))/2.0)/frameSynchedZoom_;
	float y = (viewportPoint.y()-(height_-(height_*frameSynchedZoom_))/2.0)/frameSynchedZoom_;
	return QPoint(x,y);
}

QPoint PixmapVisualTarget::targetPointToViewportPoint(QPoint targetPoint)
{
	float x = (frameSynchedZoom_*targetPoint.x())+(width_-(width_ *frameSynchedZoom_))/2.0;
	float y = (frameSynchedZoom_*targetPoint.y())+(height_-(height_*frameSynchedZoom_))/2.0;
	return QPoint(x,y);

}

void PixmapVisualTarget::clear()
{
	pixmapCompositingSurfaces_[~surfaceActingAsBackBuffer_ & 1].fill(QColor(0,0,0,0));

	//Set first phosphor time !! THIS IS NOT ACCURATE YET
	setFirstPhosphorTime();
	repaint();
}

}; //namespace Picto
