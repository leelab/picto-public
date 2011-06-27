#include "CircleGraphic.h"

#include <QPainter>

namespace Picto {

const QString CircleGraphic::type = "Circle Graphic";

CircleGraphic::CircleGraphic(QPoint position, int radius, QColor color)
{
	
	AddDefinableProperty(QVariant::Point,"Position",position);
	AddDefinableProperty(QVariant::Color,"Color",color);

	QMap<QString,QVariant> attributeMap;
	attributeMap["minimum"] = 1;
	attributeMap["maximum"] = 1000;
	attributeMap["singleStep"] = 1;
	AddDefinableProperty(QVariant::Int,"Radius",radius,attributeMap);

	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);

	//QSharedPointer<Property> radiusProperty = propertyContainer_->addProperty(QVariant::Int,"Radius",radius);
	//radiusProperty->addAttribute("minimum", 1);
	//radiusProperty->addAttribute("maximum", 1000);
	//radiusProperty->addAttribute("singleStep", 1);

	//propertyContainer_->setPropertyValue("Color",color);

	//draw();

	connect(propertyContainer_.data(),
		    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, int, QVariant))
			);
}

void CircleGraphic::draw()
{
	int radius = propertyContainer_->getPropertyValue("Radius").toInt();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(radius*2+1,radius*2+1,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.drawEllipse(image.rect());
	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* CircleGraphic::NewVisualElement()
{
	return new CircleGraphic;
}

void CircleGraphic::slotPropertyValueChanged(QString propertyName, int,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

void CircleGraphic::postSerialize()
{
	VisualElement::postSerialize();
	draw();
}

bool CircleGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
