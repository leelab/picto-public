#include <QPainter>
#include "FractalGraphic.h"
#include "../memleakdetect.h"
#include <algorithm>    // std::min_element, std::max_element

#define PI 3.14159265358979323846

namespace Picto {

	const QString FractalGraphic::type = "Fractal Graphic";

	/*! \brief Creates a new FractalGraphic object.
	*	\details Adds the following Properties:
	*	- Size: Stores the size of this graphic.
	*	- NumSuperpositions: Stores number of superpositions in this fractal.
	*	- NumEdges: Stores the number of edges in this fractal.
	*   - EdgeSize: Stores the size of the edges in this fractal.
	*   - Depth: Stores the depth of recursion in this fractal.
	*   - GA: Stores the ga variable of this fractal.
	*
	*/
	FractalGraphic::FractalGraphic(QPoint position, QRect dimensions, QColor color)
		: VisualElement(position, color)
	{
		AddDefinableProperty(QVariant::Size, "Size", dimensions.size());
		AddDefinableProperty(QVariant::Int, "NumSuperpositions", 1);
		AddDefinableProperty(QVariant::Int, "NumEdges", 4);
		AddDefinableProperty(QVariant::Int, "EdgeSize", 14);
		AddDefinableProperty(QVariant::Int, "Depth", 3);
		AddDefinableProperty(QVariant::Int, "GA", -4);
	}

	void FractalGraphic::setColor(int index, int r, int g, int b, int a)
	{
		if (index < 0 || index > colors.size())
			return;
		colors[index] = QColor(r, g, b, a);

		draw();
	}

	void FractalGraphic::generate()
	{
		int nSuperpos = propertyContainer_->getPropertyValue("NumSuperpositions").toInt();
		int temp = edgeSizes.size();
		if (edgeSizes.size() < nSuperpos)
			for (int i = 0; i < nSuperpos - temp; i++)
				edgeSizes.push_back(propertyContainer_->getPropertyValue("EdgeSize").toInt());

		temp = numEdges.size();
		if (numEdges.size() < nSuperpos)
			for (int i = 0; i < nSuperpos - temp; i++)
				numEdges.push_back(propertyContainer_->getPropertyValue("NumEdges").toInt());

		temp = depths.size();
		if (depths.size() < nSuperpos)
			for (int i = 0; i < nSuperpos - temp; i++)
				depths.push_back(propertyContainer_->getPropertyValue("Depth").toInt());

		temp = GAs.size();
		if (GAs.size() < nSuperpos)
			for (int i = 0; i < nSuperpos - temp; i++)
				GAs.push_back(propertyContainer_->getPropertyValue("GA").toInt());

		polygons.clear();

		qsrand(QTime::currentTime().msec());

		for (int i = 0; i < nSuperpos; i++)
		{
			std::vector<double> angles, xs, ys, x, y;
			int edgeSize = edgeSizes[i] - 2 * (i + 1);
			int nEdges = numEdges[i];
			int depth = depths[i];
			int GA = GAs[i];

			for (int j = 0; j < nEdges; j++)
			{
				angles.push_back(2 * PI / nEdges*(j + 1));
				xs.push_back(std::sinf(angles[j])*edgeSize);
				ys.push_back(std::cosf(angles[j])*edgeSize);
			}

			for (int j = 0; j < depth; j++)
			{
				x.clear();
				y.clear();
				for (int k = 0; k < xs.size(); k++)
				{
					double mx, my, dx, dy, theta;

					if (k == 0)
					{
						mx = (xs.back() + xs.front()) / 2;
						my = (ys.back() + ys.front()) / 2;
						dx = xs.front() - xs.back();
						dy = ys.front() - ys.back();
					}
					else
					{
						mx = (xs[k] + xs[k - 1]) / 2;
						my = (ys[k] + ys[k - 1]) / 2;
						dx = xs[k] - xs[k - 1];
						dy = ys[k] - ys[k - 1];
					}
					theta = std::atanf(dx / dy);
					if (dy < 0)
						theta += PI;
					if (fabs(dy) < 0.001)
						theta = PI / 2 * 3;

					x.push_back(mx + GA * std::sinf(theta - PI / 2));
					x.push_back(xs[k]);
					y.push_back(my + GA * std::cosf(theta - PI / 2));
					y.push_back(ys[k]);
				}
				xs = x;
				ys = y;
			}
			QPolygonF poly;
			for (int j = 0; j < xs.size(); j++)
			{
				poly << QPointF(xs[j] * 10.f, ys[j] * 10.f);
			}
			polygons.push_back(poly);

			if (colors.size() < polygons.size())
				colors.push_back(QColor::fromRgba(qRgba(qrand() % 256, qrand() % 256, qrand() % 256, 200)));
		}
	}

	void FractalGraphic::draw()
	{
		QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();
		QRectF dimensions, size = QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize());

		generate();

		dimensions = polygons.front().boundingRect();

		int defaultWidth = dimensions.width();
		int defaultHeight = dimensions.height();
		int max = max(defaultWidth, defaultHeight);
		double scaleX = size.width() / dimensions.width();
		double scaleY = size.height() / dimensions.height();

		QRectF imageDimensions = QRectF(QPointF(-1.0 * max, -1.0 * max), QPointF(2.0 * max, 2.0 * max));
		QImage image(imageDimensions.width(), imageDimensions.height(), QImage::Format_ARGB32);
		image.fill(0);
		posOffset_ = QPoint(-1.0*imageDimensions.left(), -1.0*imageDimensions.top());

		QPainter p(&image);
		QPen pen(color);

		p.setPen(pen);
		p.setRenderHint(QPainter::Antialiasing, true);

		QTransform transform;
		transform.translate(max, max);
		transform.rotate(360.);

		if (scaleX > 0 && scaleY > 0)
			transform.scale(scaleX, scaleY);

		p.setTransform(transform);
		
		for (int i = 0; i < polygons.size(); i++)
		{
			pen.setColor(colors[i]);
			p.setPen(pen);
			p.setBrush(colors[i]);
			p.drawPolygon(polygons[i]);
		}
		
		p.end();
		image_ = image;

		shouldUpdateCompositingSurfaces_ = true;
	}

	/*! \brief Creates a new FractalGraphic object and returns a shared Asset pointer to it.*/
	QSharedPointer<Asset> FractalGraphic::Create()
	{
		return QSharedPointer<Asset>(new FractalGraphic());
	}

	QPoint FractalGraphic::getPositionOffset()
	{
		return posOffset_;
	}

	void FractalGraphic::postDeserialize()
	{
		VisualElement::postDeserialize();
		setPropertyRuntimeEditable("NumSuperpositions");
		setPropertyRuntimeEditable("NumEdges");
		setPropertyRuntimeEditable("EdgeSize");
		setPropertyRuntimeEditable("Depth");
		setPropertyRuntimeEditable("GA");
		setPropertyRuntimeEditable("Size");

		propertyContainer_->getProperty("Color")->setVisible(false);
	}

	bool FractalGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
	{
		if (!VisualElement::validateObject(xmlStreamReader))
			return false;
		if (propertyContainer_->getPropertyValue("NumSuperpositions").toInt() < 1)
		{
			addError("NumSuperpositions must be greater than 1.");
			return false;
		}
		if (propertyContainer_->getPropertyValue("NumEdges").toInt() < 3)
		{
			addError("NumEdges must be greater than 2.");
			return false;
		}
		if (propertyContainer_->getPropertyValue("EdgeSize").toInt() < (2 * getSuperposition()))
		{
			addError("EdgeSize must not be smaller than NumSuperpositions * 2.");
			return false;
		}
		if (propertyContainer_->getPropertyValue("Depth").toInt() < 1)
		{
			addError("Depth must be greater than 0.");
			return false;
		}
		if (propertyContainer_->getPropertyValue("GA").toInt() == 0)
		{
			addError("GA must not equal 0.");
			return false;
		}
		return true;
	}

}; //namespace Picto
