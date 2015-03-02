#include <QPainter>
#include "EllipseGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString EllipseGraphic::type = "Ellipse Graphic";

/*! \brief Creates a new EllipseGraphic object at the input position and with the input dimensions and color.
 *	\details Adds the following Properties:
 *	- Outline: Stores whether only the graphics outline will be shown.
 *	- OutlineThickness: Stores the thickness of the outline that will be shown if "Outline" is true.
 *	- Size: Stores the size of the graphic (width, height).
 */
EllipseGraphic::EllipseGraphic(QPoint position, QRect dimensions, QColor color)
: VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
	AddDefinableProperty(QVariant::Size,"Size",dimensions.size());
}

void EllipseGraphic::draw()
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
	p.drawEllipse(dimensions);
	p.end();
	image_ = image;
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief Creates a new EllipseGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> EllipseGraphic::Create()
{
	return QSharedPointer<Asset>(new EllipseGraphic());
}

QPoint EllipseGraphic::getPositionOffset()
{
	return posOffset_;
}

void EllipseGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool EllipseGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
