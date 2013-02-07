#include <QPainter>
#include "ShapeShifterGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

const QString ShapeShifterGraphic::type = "Shape Shifter Graphic";

ShapeShifterGraphic::ShapeShifterGraphic(QPoint position, QRect dimensions, QColor color)
: VisualElement(position,color)
{
	shapeList_ << "Ellipse" << "Rectangle" << "Diamond";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"Shape",0,"enumNames",shapeList_);
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
	AddDefinableProperty(QVariant::Rect,"Dimensions",dimensions);
}
QRect ShapeShifterGraphic::getDimensions()
{
	return propertyContainer_->getPropertyValue("Dimensions").toRect();
}

void ShapeShifterGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Dimensions",dimensions);
}

QPoint ShapeShifterGraphic::getPositionOffset()
{
	return posOffset_;
}

QString ShapeShifterGraphic::getShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"");
}

void ShapeShifterGraphic::setShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("Shape",shapeList_.indexOf(shape));
}

void ShapeShifterGraphic::draw()
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
	
	if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") == "Ellipse")
	{
		p.drawEllipse(dimensions);
	}
	else if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") == "Rectangle")
	{
		p.drawRect(dimensions);
	}
	else if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") == "Diamond")
	{
		QPolygon diamond;
		int maxWPoint = dimensions.width()-1;
		int maxHPoint = dimensions.height()-1;
		diamond << QPoint(maxWPoint/2,0)
				<< QPoint(maxWPoint,maxHPoint/2)
				<< QPoint(maxWPoint/2,maxHPoint)
				<< QPoint(0,maxHPoint/2)
				<< QPoint(maxWPoint/2,0);
		p.drawPolygon(diamond);
	}

	p.end();
	image_ = image;
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* ShapeShifterGraphic::NewVisualElement()
{
	return new ShapeShifterGraphic;
}

QSharedPointer<Asset> ShapeShifterGraphic::Create()
{
	return QSharedPointer<Asset>(new ShapeShifterGraphic());
}

//void ShapeShifterGraphic::slotPropertyValueChanged(QString propertyName, int,
//											  QVariant) //propertyValue
//{
//	if(propertyName != "Position" && propertyName != "Name")
//	{
//		draw();
//	}
//}

void ShapeShifterGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool ShapeShifterGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
