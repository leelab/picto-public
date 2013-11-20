#include <QPainter>
#include "CircleTarget.h"
namespace Picto
{

CircleTarget::CircleTarget()
:
ControlTarget(),
active_(false)
{
	AddDefinableProperty(QVariant::Int,"Radius",1);
}
/*! \brief Creates and returns a shared pointer to a CircleTarget*/
QSharedPointer<Asset> CircleTarget::Create()
{
	return QSharedPointer<Asset>(new CircleTarget());
}

void CircleTarget::draw()
{
	int radius = propertyContainer_->getPropertyValue("Radius").toInt();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(radius*2+2,radius*2+2,QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,0));
	QPen pen(color);
	if(getOutside())
		pen.setStyle(Qt::DotLine);
	p.setPen(pen);
	QRect ellipseRect(1,1,radius*2,radius*2);
	p.drawEllipse(ellipseRect);
	p.end();
	image_ = image;
	posOffset_ = QPoint(radius+1,radius+1);

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

bool CircleTarget::contains(int x, int y)
{
	int radius = propertyContainer_->getPropertyValue("Radius").toInt();
	QPoint center = (getPosition()-getPositionOffset())+QPoint(radius,radius);
	QPoint offset = QPoint(x,y)-center;
	if( ( ((offset.rx()*offset.rx())+(offset.ry()*offset.ry())) > (radius * radius) ) )
		return getOutside()?true:false;
	return getOutside()?false:true;
}

QPoint CircleTarget::getPositionOffset()
{
	return posOffset_;
}
/*! \brief Gets the circle target's current radius*/
int CircleTarget::getRadius()
{
	return propertyContainer_->getPropertyValue("Radius").toInt();
}
/*! \brief Sets the circle target's current radius*/
void CircleTarget::setRadius(int radius)
{
	propertyContainer_->setPropertyValue("Radius",radius);
}

void CircleTarget::postDeserialize()
{
	ControlTarget::postDeserialize();
	setPropertyRuntimeEditable("Radius");
}


bool CircleTarget::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlTarget::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto