#include "LineGraphic.h"

#include <QPainter>

namespace Picto {

LineGraphic::LineGraphic(QPoint position, QVector<QPoint> points, QColor color)
{
	parameterContainer_.setContainerName("Line Graphic");

	parameterContainer_.addParameter(Parameter(QVariant::Point,"Position",position));

	for(int i = 0; i < points.count(); i++)
	{
		Parameter pointParameter(QVariant::Point,QString("Point %1").arg(i),points[i]);
		parameterContainer_.addParameter(pointParameter);
	}

	parameterContainer_.addParameter(Parameter(QVariant::Color,"Color",color));

	parameterContainer_.addParameter(Parameter(QVariant::String,"Name",""));

	draw();

	connect(&parameterContainer_,
		    SIGNAL(signalParameterValueChanged(QString, QVariant)),
		    this,
			SLOT(slotParameterValueChanged(QString, QVariant))
			);
}

void LineGraphic::draw()
{
	QVector<QPoint> points;
	int left = 0, top = 0, right = 0, bottom = 0;

	foreach(QString parameterName, parameterContainer_.getParameterList())
	{
		if(parameterName.left(5)=="Point")
		{
			QPoint point = parameterContainer_.getParameterValue(parameterName).toPoint();
			points.push_back(point);

			if(point.x() < left) left = point.x();
			if(point.y() > right) right = point.x();
			if(point.y() < top) top = point.y();
			if(point.y() > bottom) bottom = point.y();
		}
	}

	QColor color = parameterContainer_.getParameterValue("Color").value<QColor>();

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

	shouldRedrawImage_ = false;
}

void LineGraphic::slotParameterValueChanged(QString parameterName,
											  QVariant) //parameterValue
{
	if(parameterName != "Position" && parameterName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
