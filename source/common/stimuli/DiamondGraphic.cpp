#include <QPainter>
#include "DiamondGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief I believe that this is no longer used.  It sbould probably be deleted.*/
const QString DiamondGraphic::type = "Diamond Graphic";

/*! \brief Creates a new DiamondGraphic object at the input position and with the input dimensions and color.
 *	\details Adds the following Properties:
 *	- Outline: Stores whether only the graphics outline will be shown.
 *	- OutlineThickness: Stores the thickness of the outline that will be shown if "Outline" is true.
 *	- Size: Stores the size of the graphic (width, height).
 */
DiamondGraphic::DiamondGraphic(QPoint position, QRect dimensions, QColor color)
: VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
	AddDefinableProperty(QVariant::Size,"Size",dimensions.size());
}

/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
 *	\note The returned value contains no position information.  Only the Width, Height
 *	values should be used.
*/
QRect DiamondGraphic::getDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}

/*! \brief Set the dimensions (width, height) of this graphic.
 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
*/
void DiamondGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Size",dimensions.size());
}

QPoint DiamondGraphic::getPositionOffset()
{
	return posOffset_;
}

void DiamondGraphic::draw()
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
	QPolygon diamond;
	int maxWPoint = dimensions.width()-1;
	int maxHPoint = dimensions.height()-1;
	diamond << QPoint(maxWPoint/2,0)
			<< QPoint(maxWPoint,maxHPoint/2)
			<< QPoint(maxWPoint/2,maxHPoint)
			<< QPoint(0,maxHPoint/2)
			<< QPoint(maxWPoint/2,0);
	p.drawPolygon(diamond);
	p.end();
	image_ = image;
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief This is no longer used by parts of Picto that are being used.  It sbould probably be deleted.*/
VisualElement* DiamondGraphic::NewVisualElement()
{
	return new DiamondGraphic;
}

/*! \brief Creates a new DiamondGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> DiamondGraphic::Create()
{
	return QSharedPointer<Asset>(new DiamondGraphic());
}

void DiamondGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool DiamondGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
