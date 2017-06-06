#include <QPainter>
#include "BoxGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString BoxGraphic::type = "Box Graphic";

	/*! \brief Creates a new BoxGraphic object at the input position and with the input dimensions and color.
	*	\details Adds the following Properties:
	*	- Outline: Stores whether only the graphics outline will be shown.
	*	- OutlineThickness: Stores the thickness of the outline that will be shown if "Outline" is true.
	*	- Size: Stores the size of the graphic (width, height).
	*/
	BoxGraphic::BoxGraphic(QPoint position, QRect dimensions, QColor color)
		: VisualElement(position, color)
	{
		AddDefinableProperty(QVariant::Bool, "Outline", false);
		AddDefinableProperty(QVariant::Int, "OutlineThickness", 0);
		AddDefinableProperty(QVariant::Size, "Size", dimensions.size());
		AddDefinableProperty(QVariant::Double, "Angle", 0.);
	}
	/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
	*	\note The returned value contains no position information.  Only the Width, Height
	*	values should be used.
	*/
	QRect BoxGraphic::getDimensions()
	{
		return QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());
	}

	/*! \brief Set the dimensions (width, height) of this graphic.
	*	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
	*/
	void BoxGraphic::setDimensions(QRect dimensions)
	{
		propertyContainer_->setPropertyValue("Size", dimensions.size());
	}

	QPoint BoxGraphic::getPositionOffset()
	{
		return posOffset_;
	}
	
	void BoxGraphic::draw()
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
		
		p.drawRect(dimensions);
		p.end();

		image_ = image;
	
		shouldUpdateCompositingSurfaces_ = true;
	}

	/*! \brief Creates a new BoxGraphic object and returns a shared Asset pointer to it.*/
	QSharedPointer<Asset> BoxGraphic::Create()
	{
		return QSharedPointer<Asset>(new BoxGraphic());
	}

	void BoxGraphic::postDeserialize()
	{
		VisualElement::postDeserialize();
	}

	bool BoxGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
	{
		if (!VisualElement::validateObject(xmlStreamReader))
			return false;
		return true;
	}

}; //namespace Picto
