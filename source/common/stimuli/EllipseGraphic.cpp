#include "EllipseGraphic.h"

#include <QPainter>

namespace Picto {

const QString EllipseGraphic::type = "Ellipse Graphic";

EllipseGraphic::EllipseGraphic(QPoint position, QRect dimensions, QColor color)
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

void EllipseGraphic::draw()
{
	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.drawEllipse(dimensions);
	p.end();
	image_ = image;

	shouldUpdateCompositingSurfaces_ = true;
}

VisualElement* EllipseGraphic::NewVisualElement()
{
	return new EllipseGraphic;
}

void EllipseGraphic::setHeight(int height)
{
	QRect origDims = getDimensions();
	origDims.setHeight(height);
	setDimensions(origDims);
}

void EllipseGraphic::setWidth(int width)
{
	QRect origDims = getDimensions();
	origDims.setWidth(width);
	setDimensions(origDims);
}
void EllipseGraphic::slotPropertyValueChanged(QString propertyName, int index,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}


}; //namespace Picto
