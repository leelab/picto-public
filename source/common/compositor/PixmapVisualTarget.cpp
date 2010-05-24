#include "PixmapVisualTarget.h"

#include "PixmapCompositingSurface.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>

namespace Picto {

PixmapVisualTarget::PixmapVisualTarget(bool _bWindowed, int _width, int _height) :
	VisualTarget(_bWindowed, _width, _height)
{
	if(!bWindowed_)
	{
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

void PixmapVisualTarget::draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface)
{
	if(compositingSurface->getTypeName()=="Pixmap")
	{
		QPainter painter(&pixmapCompositingSurfaces_[surfaceActingAsBackBuffer_]);

		painter.drawPixmap(location, compositingSurface.staticCast<PixmapCompositingSurface>()->getPixmap());
	}
}

void PixmapVisualTarget::present()
{
	surfaceActingAsBackBuffer_ = ~surfaceActingAsBackBuffer_ & 1;

	pixmapCompositingSurfaces_[surfaceActingAsBackBuffer_].fill(QColor(0,127,0,0));

	repaint();

	//This signal is here so that if this VisualTarget belongs to a 
	//VisualTargetHost, we will know that a repaint occured.
	emit presented();
}

void PixmapVisualTarget::drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text)
{
	QPainter painter(&pixmapCompositingSurfaces_[surfaceActingAsBackBuffer_]);

	painter.setPen(color);
	painter.setFont(font);
	painter.drawText(rect, alignment, text);
}

void PixmapVisualTarget::clear()
{
	pixmapCompositingSurfaces_[~surfaceActingAsBackBuffer_ & 1].fill(QColor(0,0,0,0));

	repaint();

	//This signal is here so that if this VisualTarget belongs to a 
	//VisualTargetHost, we will know that a repaint occured.
	emit presented();
}

}; //namespace Picto
