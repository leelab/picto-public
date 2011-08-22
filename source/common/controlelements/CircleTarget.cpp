#include "CircleTarget.h"
#include <QPainter>
namespace Picto
{

CircleTarget::CircleTarget()
:
ControlTarget(),
active_(false)
{
	AddDefinableProperty(QVariant::Int,"Radius",1);
}

void CircleTarget::draw()
{
	int radius = propertyContainer_->getPropertyValue("Radius").toInt();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(radius*2+1,radius*2+1,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,0));
	p.setPen(color);
	p.drawEllipse(image.rect());
	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

bool CircleTarget::contains(int x, int y)
{
	int radius = propertyContainer_->getPropertyValue("Radius").toInt();
	QPoint center = getPosition()+QPoint(radius,radius);
	QPoint offset = QPoint(x,y)-center;
	if( ( ((offset.rx()*offset.rx())+(offset.ry()*offset.ry())) > (radius * radius) ) )
		return false;
	return true;
}

int CircleTarget::getRadius()
{
	return propertyContainer_->getPropertyValue("Radius").toInt();
}

void CircleTarget::setRadius(int radius)
{
	propertyContainer_->setPropertyValue("Radius",radius);
}

void CircleTarget::postSerialize()
{
	ControlTarget::postSerialize();
	setPropertyRuntimeEditable("Radius");
}


bool CircleTarget::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlTarget::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto