#include "CircleGraphic.h"

#include <QPainter>

namespace Picto {

const QString CircleGraphic::type = "Circle Graphic";

CircleGraphic::CircleGraphic(QPoint position, int radius, QColor color)
{
	propertyContainer_.setContainerName(type);

	propertyContainer_.setPropertyValue("Position",position);

	Property radiusProperty(QVariant::Int,"Radius",radius);
	radiusProperty.addAttribute("minimum", 1);
	radiusProperty.addAttribute("maximum", 1000);
	radiusProperty.addAttribute("singleStep", 1);
	propertyContainer_.addProperty(radiusProperty);

	propertyContainer_.setPropertyValue("Color",color);

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


}; //namespace Picto
