#ifndef _PolygonGRAPHIC_H_
#define _PolygonGRAPHIC_H_

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
	class PICTOLIB_CLASS PolygonGraphic : public VisualElement
	{
		Q_OBJECT
			/*! \brief Sets/gets whether this graphic will display only its outline.*/
			Q_PROPERTY(bool outline READ getOutline WRITE setOutline)
	public:
		PolygonGraphic(QPoint position = QPoint(), QRect dimensions = QRect(), QColor color = QColor());
		void draw();
		static QSharedPointer<Asset> Create();
		static const QString type;

		QPoint getPositionOffset();

		bool getOutline(){ return propertyContainer_->getPropertyValue("Outline").toBool(); };
		/*! \brief Sets whether this graphic should display only its outline.*/
		void setOutline(bool outline) { propertyContainer_->setPropertyValue("Outline", outline); };
		QRect getDimensions();
		void setDimensions(QRect dimensions);
		/*! \brief Returns the width of this graphic in pixels.*/
		int getWidth(){ return getDimensions().width(); };
		/*! \brief Returns the height of this graphic in pixels.*/
		int getHeight(){ return getDimensions().height(); };
		/*! \brief Sets the width of this graphic in pixels.*/
		void setWidth(int w){ QRect dims = getDimensions(); dims.setWidth(w); setDimensions(dims); };
		/*! \brief Sets the height of this graphic in pixels.*/
		void setHeight(int h){ QRect dims = getDimensions(); dims.setHeight(h); setDimensions(dims); };

		public slots:
		void setVertexX(int index, int x);
		void setVertexY(int index, int y);
		int getVertexX(int index);
		int getVertexY(int index);
		void setVertex(int index, int x, int y);
		void setDimensions(int w, int h){ setWidth(w); setHeight(h); };

		virtual QString friendlyTypeName(){ return "Polygon"; };
		virtual QString getUITemplate(){ return "PolygonElement"; };
	protected:
		virtual void postDeserialize();
		virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	private:
		QPoint posOffset_;
		QStringList listNames_;
		private slots:
		void setNumVertices(int num);
		int getNumVertices();
	};


}; //namespace Picto

#endif
