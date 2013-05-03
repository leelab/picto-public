#include <QPainter>
#include "BoxGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString BoxGraphic::type = "Box Graphic";

BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
: VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
	AddDefinableProperty(QVariant::Size,"Size",dimensions.size());



	//propertyContainer_->setContainerName(type);

	//propertyContainer_->setPropertyValue("Position",position);

	//propertyContainer_->addProperty(QVariant::Size,"Size",dimensions.size());

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
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}

void BoxGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Size",dimensions.size());
}

QPoint BoxGraphic::getPositionOffset()
{
	return posOffset_;
}

void BoxGraphic::draw()
{
	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
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

QSharedPointer<Asset> BoxGraphic::Create()
{
	return QSharedPointer<Asset>(new BoxGraphic());
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
