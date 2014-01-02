#ifndef _CIRCLEGRAPHIC_H_
#define _CIRCLEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Represents a circle graphic (ie. a rectangle).
 *
 *	\details In addition to the Properties provided by the VisualElement class, this class provides radius, outline and outline thickness
 *	Properties.  The radius is in defined in pixels.  The outline Property determines if only the outline of the
 *	graphic will be drawn.  The outline thickness Property determines the thickness of the graphic outline if the outline Property is true.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PICTOLIB_CLASS CircleGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the radius of the circle in pixels.*/
	Q_PROPERTY(int radius READ getRadius WRITE setRadius)
	/*! \brief Sets/gets whether this graphic will display only its outline.*/
	Q_PROPERTY(bool outline READ getOutline WRITE setOutline)
public:
	CircleGraphic(QPoint position=QPoint(), int radius=0, QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	static const QString type;
	QPoint getPositionOffset();
	/*! \brief Returns the radius of the circle in pixels.*/
	int getRadius(){return propertyContainer_->getPropertyValue("Radius").toInt();};
	/*! \brief Sets the radius of the circle in pixels.*/
	void setRadius(int radius) { propertyContainer_->setPropertyValue("Radius", radius);};
	/*! \brief Returns whether this graphic is set to display only its outline.*/
	bool getOutline(){return propertyContainer_->getPropertyValue("Outline").toBool();};
	/*! \brief Sets whether this graphic should display only its outline.*/
	void setOutline(bool outline) { propertyContainer_->setPropertyValue("Outline", outline);};

	virtual QString friendlyTypeName(){return "Circle";};
	virtual QString getUITemplate(){return "CircleElement";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QPoint posOffset_;
private slots:
	
};


}; //namespace Picto

#endif
