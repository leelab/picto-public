#include "BoxGraphic.h"

#include <QPainter>

namespace Picto {

const QString BoxGraphic::type = "Box Graphic";

BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
: VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
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

QPoint BoxGraphic::getPositionOffset()
{
	return posOffset_;
}

void BoxGraphic::draw()
{
	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
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
	p.drawRect(dimensions);
	p.end();
	image_ = image;
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
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

void BoxGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool BoxGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
