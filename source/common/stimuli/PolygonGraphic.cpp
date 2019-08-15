#include <QPainter>
#include "PolygonGraphic.h"
#include "../memleakdetect.h"
#include <algorithm>    // std::min_element, std::max_element


namespace Picto {

	const QString PolygonGraphic::type = "Polygon Graphic";

	/*! \brief Creates a new PolygonGraphic object.
	*	\details Adds the following Properties:
	*	- Outline: Stores whether only the graphics outline will be shown.
	*	- OutlineThickness: Stores the thickness of the outline that will be shown if "Outline" is true.
	*	- Vertices: Stores the vertices of the Polygon.
	*
	*/
	PolygonGraphic::PolygonGraphic(QPoint position, QRect dimensions, QColor color)
		: VisualElement(position, color)
	{
		AddDefinableProperty(QVariant::Bool, "Outline", false);
		AddDefinableProperty(QVariant::Int, "OutlineThickness", 0);
		AddDefinableProperty(QVariant::Size, "Size", dimensions.size());
		AddDefinableProperty(QVariant::Int, "NumVertices", 0);
	}
	/*! \brief Sets the number of Labels to be displayed.
	*/
	void PolygonGraphic::setNumVertices(int num)
	{
		if (num < 0)
			num = 0;
		propertyContainer_->setPropertyValue("NumVertices", num);
	}

	/*! \brief Gets the number of Labels to be displayed.
	*/
	int PolygonGraphic::getNumVertices()
	{
		return propertyContainer_->getPropertyValue("NumVertices").toInt();
	}
	/*! \brief Sets the local x position of the Vertex at the input index
	*/
	void PolygonGraphic::setVertex(int index, int x, int y)
	{
		setVertexX(index,x);
		setVertexY(index, y);
	}
	/*! \brief Sets the local x position of the Vertex at the input index
	*/
	void PolygonGraphic::setVertexX(int index, int x)
	{
		if (index < 0 || index >= getNumVertices())
			return;
		QStringList xs = propertyContainer_->getPropertyValue("VertexXs").toString().split(",", QString::SkipEmptyParts);
		while (xs.size() <= index)
		{
			xs.push_back("_");
		}
		xs[index] = QString::number(x);
		propertyContainer_->setPropertyValue("VertexXs", xs.join(","));
	}

	/*! \brief Sets the local y position of the Label at the input index
	*/
	void PolygonGraphic::setVertexY(int index, int y)
	{
		if (index < 0 || index >= getNumVertices())
			return;
		QStringList ys = propertyContainer_->getPropertyValue("VertexYs").toString().split(",", QString::SkipEmptyParts);
		while (ys.size() <= index)
		{
			ys.push_back("_");
		}
		ys[index] = QString::number(y);
		propertyContainer_->setPropertyValue("VertexYs", ys.join(","));
	}
	/*! \brief Returns the x position component of the Vertex at the input index.
	*/
	int PolygonGraphic::getVertexX(int index)
	{
		QStringList xs = propertyContainer_->getPropertyValue("VertexXs").toString().split(",", QString::SkipEmptyParts);
		if (index < 0 || index >= xs.size())
			return 0;
		QString val = xs[index];
		if (val == "_")
			return 0;
		return val.toInt();
	}

	/*! \brief Returns the y position component of the Vertex at the input index.
	*/
	int PolygonGraphic::getVertexY(int index)
	{
		QStringList ys = propertyContainer_->getPropertyValue("VertexYs").toString().split(",", QString::SkipEmptyParts);
		if (index < 0 || index >= ys.size())
			return 0;
		QString val = ys[index];
		if (val == "_")
			return 0;
		return val.toInt();
	}

	void PolygonGraphic::draw()
	{
		QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();
		QRect dimensions = QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());

		int numVertices = getNumVertices();
		QStringList xs = propertyContainer_->getPropertyValue("VertexXs").toString().split(",", QString::SkipEmptyParts);
		QStringList ys = propertyContainer_->getPropertyValue("VertexYs").toString().split(",", QString::SkipEmptyParts);

		if ((xs.size() != numVertices)
			|| (ys.size() != numVertices)
			)
		{
			//Stuff is in the process of being updated.
			return;
		}

		for (int i = 0; i < numVertices; i++)
		{
			if (xs[i] == "_")
				xs[i] = QString("0");
			if (ys[i] == "_")
				ys[i] = QString("0");
		}
			
		
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
		transform.rotate(360.);
		transform.translate(-1.0*xc, -1.0*yc);
		p.setTransform(transform);



		QPolygon polygon;
		for (int i = 0; i < numVertices; i++)
		{
			int posX = 0; //default
			if (xs.size() > i  && xs[i] != "_")
				posX = xs[i].toDouble();

			int posY = 0; //default
			if (ys.size() > i  && ys[i] != "_")
				posY = ys[i].toDouble();

			polygon << QPoint(posX, posY);
		}


		p.drawPolygon(polygon);
		p.end();
		image_ = image;

		shouldUpdateCompositingSurfaces_ = true;
	}

	/*! \brief Creates a new PolygonGraphic object and returns a shared Asset pointer to it.*/
	QSharedPointer<Asset> PolygonGraphic::Create()
	{
		return QSharedPointer<Asset>(new PolygonGraphic());
	}

	QPoint PolygonGraphic::getPositionOffset()
	{
		return posOffset_;
	}

	/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
	*	\note The returned value contains no position information.  Only the Width, Height
	*	values should be used.
	*/
	QRect PolygonGraphic::getDimensions()
	{
		return QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());
	}

	/*! \brief Set the dimensions (width, height) of this graphic.
	*	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
	*/
	void PolygonGraphic::setDimensions(QRect dimensions)
	{
		propertyContainer_->setPropertyValue("Size", dimensions.size());
	}

	void PolygonGraphic::postDeserialize()
	{
		VisualElement::postDeserialize();
	}

	bool PolygonGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
	{
		if (!VisualElement::validateObject(xmlStreamReader))
			return false;
		return true;
	}

}; //namespace Picto
