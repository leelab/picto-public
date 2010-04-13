#include "LineGraphic.h"

#include <QPainter>

namespace Picto {

const QString LineGraphic::type = "Line Graphic";

LineGraphic::LineGraphic(QPoint position, QVector<QPoint> points, QColor color)
{
	propertyContainer_.setContainerName(type);

	propertyContainer_.setPropertyValue("Position",position);
	
	for(int i = 0; i < points.count(); i++)
	{
		Property pointProperty(QVariant::Point,QString("Point %1").arg(i),points[i]);
		propertyContainer_.addProperty(pointProperty);
	}

	propertyContainer_.setPropertyValue("Color",color);

	draw();

	connect(&propertyContainer_,
		    SIGNAL(signalPropertyValueChanged(QString, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, QVariant))
			);
}

void LineGraphic::draw()
{
	QVector<QPoint> points;
	int left = 0, top = 0, right = 0, bottom = 0;

	foreach(QString propertyName, propertyContainer_.getPropertyList())
	{
		if(propertyName.left(5)=="Point")
		{
			QPoint point = propertyContainer_.getPropertyValue(propertyName).toPoint();
			points.push_back(point);

			if(point.x() < left) left = point.x();
			if(point.x() > right) right = point.x();
			if(point.y() < top) top = point.y();
			if(point.y() > bottom) bottom = point.y();
		}
	}

	QColor color = propertyContainer_.getPropertyValue("Color").value<QColor>();

	QImage image(right-left,bottom-top,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	for(int i = 1; i < points.count(); i++)
	{
		p.drawLine(points[i-1],points[i]);
	}
	p.end();
	image_ = image;

	updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = false;
}

VisualElement* LineGraphic::NewVisualElement()
{
	return new LineGraphic;
}

void LineGraphic::slotPropertyValueChanged(QString propertyName,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
