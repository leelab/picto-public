#include "CircleGraphic.h"

#include <QPainter>

namespace Picto {

const QString CircleGraphic::name = "Circle Graphic";

CircleGraphic::CircleGraphic(QPoint position, int radius, QColor color)
{
	propertyContainer_.setContainerName(name);

	propertyContainer_.addProperty(Property(QVariant::Point,"Position",position));

	Property radiusProperty(QVariant::Int,"Radius",radius);
	radiusProperty.addAttribute("minimum", 1);
	radiusProperty.addAttribute("maximum", 1000);
	radiusProperty.addAttribute("singleStep", 1);
	propertyContainer_.addProperty(radiusProperty);

	propertyContainer_.addProperty(Property(QVariant::Color,"Color",color));

	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));

	draw();

	connect(&propertyContainer_,
		    SIGNAL(signalPropertyValueChanged(QString, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, QVariant))
			);
}

void CircleGraphic::draw()
{
	int radius = propertyContainer_.getPropertyValue("Radius").toInt();
	QColor color = propertyContainer_.getPropertyValue("Color").value<QColor>();

	QImage image(radius*2+1,radius*2+1,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.drawEllipse(image.rect());
	p.end();
	image_ = image;

	updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = false;
}

VisualElement* CircleGraphic::NewVisualElement()
{
	return new CircleGraphic;
}

void CircleGraphic::slotPropertyValueChanged(QString propertyName,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}


bool CircleGraphic::deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader)
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
		else if(xmlStreamReader->name() == "Radius")
		{
			int radius = xmlStreamReader->readElementText().toInt();
			propertyContainer_.setPropertyValue("Radius",radius);
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

}; //namespace Picto
