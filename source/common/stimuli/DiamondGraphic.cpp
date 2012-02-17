#include <QPainter>
#include "DiamondGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString DiamondGraphic::type = "Diamond Graphic";

DiamondGraphic::DiamondGraphic(QPoint position, QRect dimensions, QColor color)
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
QRect DiamondGraphic::getDimensions()
{
	return propertyContainer_->getPropertyValue("Dimensions").toRect();
}

void DiamondGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Dimensions",dimensions);
}

QPoint DiamondGraphic::getPositionOffset()
{
	return posOffset_;
}

void DiamondGraphic::draw()
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
	QPolygon diamond;
	diamond << QPoint(dimensions.width()/2,0)
			<< QPoint(dimensions.width(),dimensions.height()/2)
			<< QPoint(dimensions.width()/2,dimensions.height())
			<< QPoint(0,dimensions.height()/2)
			<< QPoint(dimensions.width()/2,0);
	p.drawPolygon(diamond);
	p.end();
	image_ = image;
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* DiamondGraphic::NewVisualElement()
{
	return new DiamondGraphic;
}

QSharedPointer<Asset> DiamondGraphic::Create()
{
	return QSharedPointer<Asset>(new DiamondGraphic());
}

//void DiamondGraphic::slotPropertyValueChanged(QString propertyName, int,
//											  QVariant) //propertyValue
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

void DiamondGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool DiamondGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
