#include "BoxGraphic.h"

#include <QPainter>

namespace Picto {

const QString BoxGraphic::name = "Box Graphic";

BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
{
	propertyContainer_.setContainerName(name);

	propertyContainer_.addProperty(Property(QVariant::Point,"Position",position));

	Property dimensionsProperty(QVariant::Rect,"Dimensions",dimensions);
	propertyContainer_.addProperty(dimensionsProperty);

	propertyContainer_.addProperty(Property(QVariant::Color,"Color",color));

	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));

	draw();

	connect(&propertyContainer_,
		    SIGNAL(signalPropertyValueChanged(QString, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, QVariant))
			);
}

void BoxGraphic::draw()
{
	QRect dimensions = propertyContainer_.getPropertyValue("Dimensions").toRect();
	QColor color = propertyContainer_.getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.drawRect(dimensions);
	p.end();
	image_ = image;

	updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = false;
}

VisualElement* BoxGraphic::NewVisualElement()
{
	return new BoxGraphic;
}

void BoxGraphic::slotPropertyValueChanged(QString propertyName,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

bool BoxGraphic::deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader)
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
		else if(xmlStreamReader->name() == "Dimensions")
		{
			QRect dimensions = deserializeQRect(xmlStreamReader);
			propertyContainer_.setPropertyValue("Dimensions",dimensions);
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
