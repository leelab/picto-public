#include "BoxGraphic.h"

#include <QPainter>

namespace Picto {

BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
{
	propertyContainer_.setContainerName("Box Graphic");

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

	shouldRedrawImage_ = false;
}

void BoxGraphic::slotPropertyValueChanged(QString propertyName,
											  QVariant) //propertyValue
{
	if(propertyName != "Position" && propertyName != "Name")
	{
		draw();
	}
}

}; //namespace Picto
