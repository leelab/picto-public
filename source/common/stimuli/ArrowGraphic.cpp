#include "ArrowGraphic.h"

#include <QPainter>

namespace Picto {

const QString ArrowGraphic::name = "Arrow Graphic";

ArrowGraphic::ArrowGraphic(QPoint position, QPoint start, QPoint end, int size, QColor color)
{
	propertyContainer_.setContainerName(name);

	propertyContainer_.addProperty(Property(QVariant::Point,"Position",position));
	propertyContainer_.addProperty(Property(QVariant::Point,"Start",start));
	propertyContainer_.addProperty(Property(QVariant::Point,"End",end));
	propertyContainer_.addProperty(Property(QVariant::Int,"Size", size));
	propertyContainer_.addProperty(Property(QVariant::Color,"Color",color));
	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));

	draw();

	connect(&propertyContainer_,
		    SIGNAL(signalPropertyValueChanged(QString, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, QVariant))
			);

}

void ArrowGraphic::draw()
{
	QPoint start = propertyContainer_.getPropertyValue("Start").toPoint();
	QPoint end = propertyContainer_.getPropertyValue("End").toPoint();
	int size = propertyContainer_.getPropertyValue("Size").toInt();
	QColor color = propertyContainer_.getPropertyValue("Color").value<QColor>();

	//generate the point pairs that will draw the line and arrow
	double lineAngle = atan((double)(end.y()-start.y())/(double)(end.x()-start.x()));
	double headAngle = 15.0*3.14159265/180.0;
	double headLength = 15.0+(size*3.0);

	//b = vector in opposite direction of arrow body with length of arrow heads
	QPoint b;
	if(end.x() >= start.x())
		b = QPoint(-headLength*cos(lineAngle), -headLength*sin(lineAngle));
	else
		b = QPoint(headLength*cos(lineAngle), headLength*sin(lineAngle));

	QPoint posHeadEndPoint(b.x()*cos(headAngle) + b.y()*sin(headAngle),
							-b.x()*sin(headAngle) + b.y()*cos(headAngle));
	QPoint negHeadEndPoint(b.x()*cos(-headAngle) + b.y()*sin(-headAngle),
							-b.x()*sin(-headAngle) + b.y()*cos(-headAngle));
	posHeadEndPoint.setX(posHeadEndPoint.x()+end.x());
	posHeadEndPoint.setY(posHeadEndPoint.y()+end.y());
	negHeadEndPoint.setX(negHeadEndPoint.x()+end.x());
	negHeadEndPoint.setY(negHeadEndPoint.y()+end.y());

	QVector<QPoint> points;
	points.push_back(start);
	points.push_back(end);
	points.push_back(posHeadEndPoint);
	points.push_back(end);
	points.push_back(end);
	points.push_back(negHeadEndPoint);


	int imgWidth = (start.x() > end.x()) ? start.x()+headLength : end.x()+headLength;
	int imgHeight = (start.y() > end.y()) ? start.y()+headLength : end.y()+headLength;

	//set up the image/painter
	QImage image(imgWidth,imgHeight,QImage::Format_ARGB32);
	image.fill(0x0);

	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);

	QPen pen(color);
	pen.setWidth(size);
	pen.setJoinStyle(Qt::MiterJoin);
	p.setPen(pen);

	//draw the lines
	p.drawLines(points);

	p.end();
	image_ = image;

	updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = false;
}

VisualElement* ArrowGraphic::NewVisualElement()
{
	return new ArrowGraphic;
}

void ArrowGraphic::slotPropertyValueChanged(QString propertyName, QVariant propertyValue)
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

bool ArrowGraphic::deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	bool success = true;

	//Read through the XML grabbing the properties
	//Note that we fail in the event of unexpected input...
	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "VisualElement"))
	{
		if(!xmlStreamReader->isStartElement())
		{
			//do nothing unless we're looking at a start element
		}
		else if(xmlStreamReader->name() == "Position")
		{
			QPoint position = deserializeQPoint(xmlStreamReader);
			propertyContainer_.setPropertyValue("Position", position);
		}
		else if(xmlStreamReader->name() == "Start")
		{
			QPoint start = deserializeQPoint(xmlStreamReader);
			propertyContainer_.setPropertyValue("Start", start);
		}
		else if(xmlStreamReader->name() == "End")
		{
			QPoint end = deserializeQPoint(xmlStreamReader);
			propertyContainer_.setPropertyValue("End", end);
		}
		else if(xmlStreamReader->name() == "Size")
		{
			int size = xmlStreamReader->readElementText().toInt();
			propertyContainer_.setPropertyValue("Size", size);
		}
		else if(xmlStreamReader->name() == "Color")
		{
			QColor color = deserializeQColor(xmlStreamReader);
			propertyContainer_.setPropertyValue("Color",color);
		}
		else if(xmlStreamReader->name() == "Name")
		{
			QString name = xmlStreamReader->readElementText();
			propertyContainer_.setPropertyValue("Name",name);
		}
		else
		{
			success = false;
		}
		xmlStreamReader->readNext();
	}




	draw();

	return success;
}

}