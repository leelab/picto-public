#include "BoxGraphic.h"

#include <QPainter>

namespace Picto {

const QString BoxGraphic::type = "Box Graphic";

BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
{
	propertyContainer_->setContainerName(type);

	propertyContainer_->setPropertyValue("Position",position);

	propertyContainer_->addProperty(QVariant::Rect,"Dimensions",dimensions);

	propertyContainer_->setPropertyValue("Color",color);

	draw();

	connect(propertyContainer_.data(),
		    SIGNAL(signalPropertyValueChanged(QString, int, QVariant)),
		    this,
			SLOT(slotPropertyValueChanged(QString, int, QVariant))
			);
}
QRect BoxGraphic::getDimensions()
{
	return propertyContainer_->getPropertyValue("Dimensions").toRect();
}

void BoxGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Dimensions",dimensions);
}

void BoxGraphic::setHeight(int height)
{
	QRect origDims = getDimensions();
	origDims.setHeight(height);
	setDimensions(origDims);
}

void BoxGraphic::setWidth(int width)
{
	QRect origDims = getDimensions();
	origDims.setWidth(width);
	setDimensions(origDims);
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

void BoxGraphic::slotPropertyValueChanged(QString propertyName, int index,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}



}; //namespace Picto
