#include <QPainter>
#include "RectTarget.h"
namespace Picto
{

RectTarget::RectTarget()
:
ControlTarget(),
active_(false)
{
	AddDefinableProperty(QVariant::Size,"Size",QSize());
}

void RectTarget::draw()
{
	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,0));
	QPen pen(color);
	if(getOutside())
		pen.setStyle(Qt::DotLine);
	p.setPen(pen);
	p.drawRect(dimensions);
	p.end();
	image_ = image;
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

bool RectTarget::contains(int x, int y)
{
	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	QPoint topLeft = getPosition()-getPositionOffset();
	QRect myRect(topLeft.x(),topLeft.y(),dimensions.width(),dimensions.height());
	if(myRect.contains(x,y))
		return getOutside()?false:true;
	return getOutside()?true:false;
}

/*! \brief Constructs and returns a shared pointer to a new RectTarget.*/
QSharedPointer<Asset> RectTarget::Create()
{
	return QSharedPointer<Asset>(new RectTarget());
}

/*! \brief Returns the dimensions of this elements target rectangle.
*/
QRect RectTarget::getBounds()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}
/*! \brief Sets the target's current width.*/
void RectTarget::setWidth(int width)
{
	QRect bounds = getBounds();
	bounds.setRect(bounds.x(),bounds.y(),width,bounds.height());
	setBounds(bounds);
}
/*! \brief Sets the target's current height.*/
void RectTarget::setHeight(int height)
{
	QRect bounds = getBounds();
	bounds.setRect(bounds.x(),bounds.y(),bounds.width(),height);
	setBounds(bounds);
}

QPoint RectTarget::getPositionOffset()
{
	return posOffset_;
}

/*! \brief Sets the dimensions of the target rectangle to those of the input bounds rectangle
*/
void RectTarget::setBounds(QRect bounds)
{
	propertyContainer_->setPropertyValue("Size",bounds.size());
}

void RectTarget::postDeserialize()
{
	ControlTarget::postDeserialize();
	setPropertyRuntimeEditable("Size");
}


bool RectTarget::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlTarget::validateObject(xmlStreamReader))
		return false;
	return true;
}

}//namespace Picto