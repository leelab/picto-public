#include "BoxGraphic.h"

#include <QPainter>

namespace Picto {

const QString BoxGraphic::type = "Box Graphic";

BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
: VisualElement(position,color)
{
	
	AddDefinableProperty(QVariant::Rect,"Dimensions",dimensions);



	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);

	//propertyContainer_->addProperty(QVariant::Rect,"Dimensions",dimensions);

	//propertyContainer_->setPropertyValue("Color",color);

	//draw();

	//connect(propertyContainer_.data(),
	//	    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
	//	    this,
	//		SLOT(slotPropertyValueChanged(QString, int, QVariant))
	//		);
}
QRect BoxGraphic::getDimensions()
{
	return propertyContainer_->getPropertyValue("Dimensions").toRect();
}

void BoxGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Dimensions",dimensions);
}

void BoxGraphic::draw()
{
	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.drawRect(dimensions);
	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* BoxGraphic::NewVisualElement()
{
	return new BoxGraphic;
}

//void BoxGraphic::slotPropertyValueChanged(QString propertyName, int,
//											  QVariant) //propertyValue
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

void BoxGraphic::postSerialize()
{
	VisualElement::postSerialize();
}

bool BoxGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
