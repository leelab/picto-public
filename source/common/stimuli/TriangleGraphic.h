#ifndef _TriangleGRAPHIC_H_
#define _TriangleGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

	/*!	\brief Represents a Triangle graphic.
	*
	*	\details In addition to the Properties provided by the VisualElement class, this class provides radius, outline and outline thickness
	*	Properties.  The radius is in defined in pixels.  The outline Property determines if only the outline of the
	*	graphic will be drawn.  The outline thickness Property determines the thickness of the graphic outline if the outline Property is true.
	*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2018
	*/
	class PICTOLIB_CLASS TriangleGraphic : public VisualElement
	{
		Q_OBJECT
			/*! \brief Sets/gets the radius of the Triangle in pixels.*/
			Q_PROPERTY(int radius READ getRadius WRITE setRadius)
			/*! \brief Sets/gets whether this graphic will display only its outline.*/
			Q_PROPERTY(bool outline READ getOutline WRITE setOutline)
	public:
		TriangleGraphic(QPoint position = QPoint(), int radius = 0, QColor color = QColor());

		void draw();
		static QSharedPointer<Asset> Create();
		static const QString type;

		QPoint getPositionOffset();
		/*! \brief Returns the radius of the Triangle in pixels.*/
		int getRadius(){ return propertyContainer_->getPropertyValue("Radius").toInt(); };
		/*! \brief Sets the radius of the Triangle in pixels.*/
		void setRadius(int radius) { propertyContainer_->setPropertyValue("Radius", radius); };
		/*! \brief Returns whether this graphic is set to display only its outline.*/
		bool getOutline(){ return propertyContainer_->getPropertyValue("Outline").toBool(); };
		/*! \brief Sets whether this graphic should display only its outline.*/
		void setOutline(bool outline) { propertyContainer_->setPropertyValue("Outline", outline); };
		void setAngle(double angle){ propertyContainer_->setPropertyValue("Angle", angle); };
		/*! \brief Returns whether this graphic is set to display only its outline.*/
		double getAngle(){ return propertyContainer_->getPropertyValue("Angle").toDouble(); };


		virtual QString friendlyTypeName(){ return "Triangle"; };
		virtual QString getUITemplate(){ return "TriangleElement"; };
	protected:
		virtual void postDeserialize();
		virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	private:
		QPoint posOffset_;
		private slots:

	};


}; //namespace Picto

#endif
