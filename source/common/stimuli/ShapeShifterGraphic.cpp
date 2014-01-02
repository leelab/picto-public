#include <QPainter>
#include "ShapeShifterGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief I believe that this is no longer used.  It sbould probably be deleted.*/
const QString ShapeShifterGraphic::type = "Shape Shifter Graphic";

/*! \brief Creates a new ShapeShifterGraphic object at the input position and with the input dimensions and color.
 *	\details Adds the following Properties:
 *	- Shape: Stores the shape of the graphic (Ellipse, Rectangle, or Diamond).
 *	- Outline: Stores whether only the graphics outline will be shown.
 *	- OutlineThickness: Stores the thickness of the outline that will be shown if "Outline" is true.
 *	- Size: Stores the size of the graphic (width, height).
 */
ShapeShifterGraphic::ShapeShifterGraphic(QPoint position, QRect dimensions, QColor color)
: VisualElement(position,color)
{
	shapeList_ << "Ellipse" << "Rectangle" << "Diamond";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"Shape",0,"enumNames",shapeList_);
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
	AddDefinableProperty(QVariant::Size,"Size",dimensions.size());
}

/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
 *	\note The returned value contains no position information.  Only the Width, Height
 *	values should be used.
*/
QRect ShapeShifterGraphic::getDimensions()
{
	return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
}

/*! \brief Set the dimensions (width, height) of this graphic.
 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
*/
void ShapeShifterGraphic::setDimensions(QRect dimensions)
{
	propertyContainer_->setPropertyValue("Size",dimensions.size());
}

QPoint ShapeShifterGraphic::getPositionOffset()
{
	return posOffset_;
}

/*! \brief Gets the current shape of this graphic as a string ("Ellipse", "Rectangle" or "Diamond").
*/
QString ShapeShifterGraphic::getShape()
{
	return shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"");
}

/*! \brief Sets the current shape of this graphic as a string ("Ellipse", "Rectangle" or "Diamond").
*/
void ShapeShifterGraphic::setShape(QString shape)
{
	if(shapeList_.contains(shape))
		propertyContainer_->setPropertyValue("Shape",shapeList_.indexOf(shape));
}

void ShapeShifterGraphic::draw()
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
	
	if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") == "Ellipse")
	{
		p.drawEllipse(dimensions);
	}
	else if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") == "Rectangle")
	{
		p.drawRect(dimensions);
	}
	else if(shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"") == "Diamond")
	{
		QPolygon diamond;
		int maxWPoint = dimensions.width()-1;
		int maxHPoint = dimensions.height()-1;
		diamond << QPoint(maxWPoint/2,0)
				<< QPoint(maxWPoint,maxHPoint/2)
				<< QPoint(maxWPoint/2,maxHPoint)
				<< QPoint(0,maxHPoint/2)
				<< QPoint(maxWPoint/2,0);
		p.drawPolygon(diamond);
	}

	p.end();
	image_ = image;
	posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);
	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief This is no longer used by parts of Picto that are being used.  It sbould probably be deleted.*/
VisualElement* ShapeShifterGraphic::NewVisualElement()
{
	return new ShapeShifterGraphic;
}

/*! \brief Creates a new ShapeShifterGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> ShapeShifterGraphic::Create()
{
	return QSharedPointer<Asset>(new ShapeShifterGraphic());
}

void ShapeShifterGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool ShapeShifterGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
