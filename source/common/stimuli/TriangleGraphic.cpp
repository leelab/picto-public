#include <QPainter>
#include "TriangleGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString TriangleGraphic::type = "Triangle Graphic";

	/*! \brief Creates a new TriangleGraphic object at the input position and with the input radius and color.
	*	\details Adds the following Properties:
	*	- Outline: Stores whether only the graphics outline will be shown.
	*	- OutlineThickness: Stores the thickness of the outline that will be shown if "Outline" is true.
	*	- Radius: Stores the radius of the Triangle in Pixels.
	*
	* The triangle is inscribed in the virtual circle defined by radius and position inputs
	*/
	TriangleGraphic::TriangleGraphic(QPoint position, int radius, QColor color)
		: VisualElement(position, color)
	{
		AddDefinableProperty(QVariant::Bool, "Outline", false);
		AddDefinableProperty(QVariant::Int, "OutlineThickness", 0);
		QMap<QString, QVariant> attributeMap;
		AddDefinableProperty(QVariant::Int, "Radius", radius, attributeMap);
		AddDefinableProperty(QVariant::Double, "Angle", 0.);
	}

	void TriangleGraphic::draw()
	{
		int radius = propertyContainer_->getPropertyValue("Radius").toInt();
		QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();
		double angle = propertyContainer_->getPropertyValue("Angle").toDouble();

		int diam = 2 * radius;
		QSize size(diam, diam);
		QRect dimensions = QRect(QPoint(), size);

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

		QPolygonF triangle;
		float xTop = radius;
		float yTop = 0.;
		float xLeft = radius * (1.0 - ((sqrt(3)) / 2));
		float yLeft = 1.5 * radius;
		float xRight = radius * (1.0 + ((sqrt(3)) / 2));
		float yRight = 1.5 * radius;
		triangle << QPointF(xTop, yTop)
			<< QPointF(xLeft, yLeft)
			<< QPointF(xRight, yRight)
			<< QPointF(xTop, yTop);
		p.drawPolygon(triangle);
		p.end();
		image_ = image;

		shouldUpdateCompositingSurfaces_ = true;
	}

	/*! \brief Creates a new TriangleGraphic object and returns a shared Asset pointer to it.*/
	QSharedPointer<Asset> TriangleGraphic::Create()
	{
		return QSharedPointer<Asset>(new TriangleGraphic());
	}

	QPoint TriangleGraphic::getPositionOffset()
	{
		return posOffset_;
	}

	void TriangleGraphic::postDeserialize()
	{
		VisualElement::postDeserialize();
	}

	bool TriangleGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
	{
		if (!VisualElement::validateObject(xmlStreamReader))
			return false;
		return true;
	}

}; //namespace Picto
