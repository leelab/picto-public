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
	AddDefinableProperty(QVariant::Double, "Angle", 0.);
}

void EllipseGraphic::draw()
{
	QRect dimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();
	double angle = propertyContainer_->getPropertyValue("Angle").toDouble();

	int defaultWidth = dimensions.width();
	int defaultHeight = dimensions.height();
	int max = max(defaultWidth, defaultHeight);

	QRect imageDimensions = QRect(QPoint(-1.0 * max, -1.0 * max), QPoint(2.0 * max, 2.0 * max));
	QImage image(imageDimensions.width(), imageDimensions.height(), QImage::Format_ARGB32);
	image.fill(0);
	posOffset_ = QPoint(-1.0*imageDimensions.left(), -1.0*imageDimensions.top());

	QPainter p(&image);
	QPen pen(color);
	p.setBrush(color);
	if (propertyContainer_->getPropertyValue("Outline").toBool())
	{
		p.setBrush(QColor(0, 0, 0, 0));
		pen.setWidth(propertyContainer_->getPropertyValue("OutlineThickness").toInt());
	}
	p.setPen(pen);
	p.setRenderHint(QPainter::Antialiasing, true);

	qreal xc = defaultWidth * 0.5;
	qreal yc = defaultHeight * 0.5;
	QTransform transform;

	transform.translate(max, max);
	transform.rotate(angle);
	transform.translate(-1.0*xc, -1.0*yc);
	p.setTransform(transform);

	p.drawEllipse(dimensions);
	p.end();
	image_ = image;
	//posOffset_ = QPoint(dimensions.width()/2.0,dimensions.height()/2.0);

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
