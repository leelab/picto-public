#include "RectTarget.h"
#include <QPainter>
namespace Picto
{

RectTarget::RectTarget()
:
ControlTarget(),
active_(false)
{
	AddDefinableProperty(QVariant::Rect,"Dimensions",QRect());
}

void RectTarget::draw()
{
	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,0));
	p.setPen(color);
	p.drawRect(dimensions);
	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

bool RectTarget::contains(int x, int y)
{
	QRect dimensions = propertyContainer_->getPropertyValue("Dimensions").toRect();
	QPoint topLeft = getPosition();
	QRect myRect(topLeft.x(),topLeft.y(),dimensions.width(),dimensions.height());
	if(myRect.contains(x,y))
		return true;
	return false;
}

QRect RectTarget::getBounds()
{
	return propertyContainer_->getPropertyValue("Dimensions").toRect();
}

void RectTarget::setWidth(int width)
{
	QRect bounds = getBounds();
	bounds.setRect(bounds.x(),bounds.y(),width,bounds.height());
	setBounds(bounds);
}
void RectTarget::setHeight(int height)
{
	QRect bounds = getBounds();
	bounds.setRect(bounds.x(),bounds.y(),bounds.width(),height);
	setBounds(bounds);
}

void RectTarget::setBounds(QRect bounds)
{
	propertyContainer_->setPropertyValue("Dimensions",bounds);
}

void RectTarget::postSerialize()
{
	ControlTarget::postSerialize();
	setPropertyRuntimeEditable("Dimensions");
}


bool RectTarget::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlTarget::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto