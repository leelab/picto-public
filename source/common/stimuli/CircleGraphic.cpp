#include <QPainter>
#include "CircleGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief I believe that this is no longer used.  It sbould probably be deleted.*/
const QString CircleGraphic::type = "Circle Graphic";

/*! \brief Creates a new CircleGraphic object at the input position and with the input radius and color.
 *	\details Adds the following Properties:
 *	- Outline: Stores whether only the graphics outline will be shown.
 *	- OutlineThickness: Stores the thickness of the outline that will be shown if "Outline" is true.
 *	- Radius: Stores the radius of the circle in Pixels.
 */
CircleGraphic::CircleGraphic(QPoint position, int radius, QColor color)
: VisualElement(position,color)
{
	AddDefinableProperty(QVariant::Bool,"Outline",false);
	AddDefinableProperty(QVariant::Int,"OutlineThickness",0);
	QMap<QString,QVariant> attributeMap;
	//attributeMap["minimum"] = 1;
	//attributeMap["maximum"] = 1000;
	//attributeMap["singleStep"] = 1;
	AddDefinableProperty(QVariant::Int,"Radius",radius,attributeMap);
}

void CircleGraphic::draw()
{
	int radius = propertyContainer_->getPropertyValue("Radius").toInt();
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	int borderWidth = propertyContainer_->getPropertyValue("Outline").toBool()?
						propertyContainer_->getPropertyValue("OutlineThickness").toInt()
						:1;
	QImage image(radius*2+borderWidth*2,radius*2+borderWidth*2,QImage::Format_ARGB32);
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
	QRect ellipseRect(borderWidth,borderWidth,radius*2,radius*2);
	p.drawEllipse(ellipseRect);
	p.end();
	image_ = image;
	posOffset_ = QPoint(radius+borderWidth,radius+borderWidth);
	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief This is no longer used by parts of Picto that are being used.  It sbould probably be deleted.*/
VisualElement* CircleGraphic::NewVisualElement()
{
	return new CircleGraphic;
}

/*! \brief Creates a new CircleGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> CircleGraphic::Create()
{
	return QSharedPointer<Asset>(new CircleGraphic());
}

QPoint CircleGraphic::getPositionOffset()
{
	return posOffset_;
}

void CircleGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
}

bool CircleGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
