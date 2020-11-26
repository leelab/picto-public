#ifndef _FractalGRAPHIC_H_
#define _FractalGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

	/*!	\brief Represents a Polygon graphic.
	*
	*	\details In addition to the Properties provided by the VisualElement class
	*	Properties. The outline Property determines if only the outline of the
	*	graphic will be drawn.
	*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2018
	*/
	class PICTOLIB_CLASS FractalGraphic : public VisualElement
	{
		Q_OBJECT
			/*! \brief Sets/gets the number of superpositions in this fractal.*/
			Q_PROPERTY(int superposition READ getSuperposition WRITE setSuperposition)
			/*! \brief Sets/gets the number of edges in this fractal.*/
			Q_PROPERTY(int edges READ getEdges WRITE setEdges)
			/*! \brief Sets/gets the depth of recursion in this fractal.*/
			Q_PROPERTY(int depth READ getDepth WRITE setDepth)
			/*! \brief Sets/gets the size of the edges in this fractal.*/
			Q_PROPERTY(int edgeSize READ getEdgeSize WRITE setEdgeSize)
			/*! \brief Sets/gets the ga varibable of this fractal.*/
			Q_PROPERTY(int ga READ getGA WRITE setGA)

	public:
		FractalGraphic(QPoint position = QPoint(), QRect dimensions = QRect(), QColor color = QColor());
		void draw();
		static QSharedPointer<Asset> Create();
		static const QString type;

		QPoint getPositionOffset();

		int getSuperposition() { return propertyContainer_->getPropertyValue("NumSuperpositions").toInt(); };
		/*! \brief Sets the number of superpositions in this fractal.*/
		void setSuperposition(int superpos) { if (superpos < 1) return; propertyContainer_->setPropertyValue("NumSuperpositions", superpos); };
		int getEdges() { return propertyContainer_->getPropertyValue("NumEdges").toInt(); };
		/*! \brief Sets the number of edges in this fractal.*/
		void setEdges(int e) { if (e < 3) return; propertyContainer_->setPropertyValue("NumEdges", e); numEdges.clear(); };
		int getDepth() { return propertyContainer_->getPropertyValue("Depth").toInt(); };
		/*! \brief Sets the depth of recursion in this fractal.*/
		void setDepth(int d) { if (d < 1) return; propertyContainer_->setPropertyValue("Depth", d); depths.clear(); };
		int getEdgeSize() { return propertyContainer_->getPropertyValue("EdgeSize").toInt(); };
		/*! \brief Sets the size of the edges in this fractal.*/
		void setEdgeSize(int s) { if (s < (getSuperposition() * 2)) return; propertyContainer_->setPropertyValue("EdgeSize", s); edgeSizes.clear(); };
		int getGA() { return propertyContainer_->getPropertyValue("GA").toInt(); };
		/*! \brief Sets the ga variable of this fractal.*/
		void setGA(int g) { if (g == 0) return; propertyContainer_->setPropertyValue("GA", g); GAs.clear(); };

		QRect getDimensions() { return QRect(QPoint(), propertyContainer_->getPropertyValue("Size").toSize()); };
		void setDimensions(QRect dimensions) { propertyContainer_->setPropertyValue("Size", dimensions.size()); };

		void generate();

		public slots:
		/*! \brief Sets the color of the fractal at index.*/
		virtual void setColor(int index, int r, int g, int b, int a = 255);
		/*! \brief Redraws the fractal with new randomized colors.*/
		void recolor() { colors.clear(); draw(); }
		/*! \brief Redraws the fractal.*/
		void redraw() { draw(); }
		/*! \brief Sets the size of this graphic.*/
		void setDimensions(int w, int h){ QRect dims = getDimensions(); dims.setWidth(w); dims.setHeight(h); setDimensions(dims); }
		/*! \brief Sets the size of the edges in this fractal.*/
		void setEdgeSize(int index, int size) { if (edgeSizes.size() > index && index >= 0) edgeSizes[index] = size; }
		/*! \brief Sets the number of edges in this fractal.*/
		void setNumEdges(int index, int num) { if (numEdges.size() > index && index >= 0) numEdges[index] = num; }
		/*! \brief Sets the depth of recursion in this fractal.*/
		void setDepth(int index, int depth) { if (depths.size() > index && index >= 0) depths[index] = depth; }
		/*! \brief Sets the ga variable of this fractal.*/
		void setGA(int index, int GA) { if (GAs.size() > index && index >= 0) GAs[index] = GA; }

		virtual QString friendlyTypeName(){ return "Fractal"; };
		virtual QString getUITemplate(){ return "FractalElement"; };
	protected:
		virtual void postDeserialize();
		virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	private:
		QPoint posOffset_;
		std::vector<QPolygonF> polygons;
		std::vector<QColor> colors;
		std::vector<int> numEdges, edgeSizes, depths, GAs;
	};


}; //namespace Picto

#endif
