#include <QPainter>

#include "ArrowGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString ArrowGraphic::type = "Arrow Graphic";

ArrowGraphic::ArrowGraphic(QPoint start, QPoint end, int size, QColor color)
: VisualElement(start,color)
{
	AddDefinableProperty(QVariant::Point,"End",end);
	AddDefinableProperty(QVariant::Int,"Thickness", size);



	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);
	//propertyContainer_->addProperty(QVariant::Point,"Start",start);
	//propertyContainer_->addProperty(QVariant::Point,"End",end);
	//propertyContainer_->addProperty(QVariant::Int,"Size", size);
	//propertyContainer_->setPropertyValue("Color",color);

	//draw();

	//connect(propertyContainer_.data(),
	//	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	//	    this,
	//		SLOT(slotPropertyValueChanged(QString, int, QVariant))
	//		);

}

void ArrowGraphic::draw()
{
	QPoint start = getPosition();
	QPoint end = propertyContainer_->getPropertyValue("End").toPoint();
	end = end-start;
	start = QPoint(0,0);
	int size = propertyContainer_->getPropertyValue("Thickness").toInt();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

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

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* ArrowGraphic::NewVisualElement()
{
	return new ArrowGraphic;
}

QSharedPointer<Asset> ArrowGraphic::Create()
{
	return QSharedPointer<Asset>(new ArrowGraphic());
}

//void ArrowGraphic::slotPropertyValueChanged(QString propertyName, int, QVariant propertyValue)
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

void ArrowGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
	propertyContainer_->getProperty("End")->setRuntimeEditable(true);
	propertyContainer_->getProperty("Thickness")->setRuntimeEditable(true);
}

bool ArrowGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}