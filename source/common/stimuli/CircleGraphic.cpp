#include <QPainter>
#include "CircleGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString CircleGraphic::type = "Circle Graphic";

CircleGraphic::CircleGraphic(QPoint position, int radius, QColor color)
: VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
	QMap<QString,QVariant> attributeMap;
	//attributeMap["minimum"] = 1;
	//attributeMap["maximum"] = 1000;
	//attributeMap["singleStep"] = 1;
	AddDefinableProperty(QVariant::Int,"Radius",radius,attributeMap);

	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);

	//QSharedPointer<Property> radiusProperty = propertyContainer_->addProperty(QVariant::Int,"Radius",radius);
	//radiusProperty->setAttribute("minimum", 1);
	//radiusProperty->setAttribute("maximum", 1000);
	//radiusProperty->setAttribute("singleStep", 1);

	//propertyContainer_->setPropertyValue("Color",color);

	//draw();

	//connect(propertyContainer_.data(),
	//	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	//	    this,
	//		SLOT(slotPropertyValueChanged(QString, int, QVariant))
	//		);
}

void CircleGraphic::draw()
{
	int radius = propertyContainer_->getPropertyValue("Radius").toInt();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	int borderWidth = propertyContainer_->getPropertyValue("Outline").toBool()?
						propertyContainer_->getPropertyValue("OutlineThickness").toInt()
						:1;
	QImage image(radius*2+borderWidth*2,radius*2+borderWidth*2,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	QPen pen(color);
	p.setBrush(color);
	if(propertyContainer_->getPropertyValue("Outline").toBool())
	{
		p.setBrush(QColor(0,0,0,0));
		pen.setWidth(propertyContainer_->getPropertyValue("OutlineThickness").toInt());
	}
	p.setPen(pen);
	p.setRenderHint(QPainter::Antialiasing, true);
	QRect ellipseRect(borderWidth,borderWidth,radius*2,radius*2);
	p.drawEllipse(ellipseRect);
	p.end();
	image_ = image;
	posOffset_ = QPoint(radius+borderWidth,radius+borderWidth);
	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* CircleGraphic::NewVisualElement()
{
	return new CircleGraphic;
}

QSharedPointer<Asset> CircleGraphic::Create()
{
	return QSharedPointer<Asset>(new CircleGraphic());
}

//void CircleGraphic::slotPropertyValueChanged(QString propertyName, int,
//											  QVariant) //propertyValue
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

QPoint CircleGraphic::getPositionOffset()
{
	return posOffset_;
}

void CircleGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool CircleGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
