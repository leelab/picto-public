#include <QColor>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QSize>
#include <QSizePolicy>

#include "StatusLight.h"


StatusLight::StatusLight(QWidget *parent, QColor color, int radius)
	: QWidget(parent),
	  currentColor(color),
	  radius(radius)
{
	setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

QSize StatusLight::sizeHint() const
{
	QSize size(radius*2,radius*2);
	return size;
}
void StatusLight::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing,true);

	int side = qMin(width(),height());

	painter.setViewport((width()-side)/2, (height()-side)/2, side, side);
	painter.setWindow(-radius-1,-radius-1,radius*2+2,radius*2+2);

	draw(&painter);
}

void StatusLight::draw(QPainter *painter)
{
	QPen pen(Qt::black,1.0);
	QBrush brush (currentColor);

	painter->setPen(pen);
	painter->setBrush(brush);
	painter->drawEllipse(-radius,-radius,radius*2,radius*2);
}
