#ifndef _DIAMONDGRAPHIC_H_
#define _DIAMONDGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Represents a diamond graphic (ie. a kit shape).
 *
 *	\details In addition to the Properties provided by the VisualElement class, this class provides size, outline and outline thickness
 *	Properties.  The size is just the total width and height of the graphic.  The outline Property determines if only the outline of the
 *	graphic will be drawn.  The outline thickness Property determines the thickness of the graphic outline if the outline Property is true.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS DiamondGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the width of this graphic in pixels.*/
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	/*! \brief Sets/gets the height of this graphic in pixels.*/
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	/*! \brief Sets/gets whether this graphic will display only its outline.*/
	Q_PROPERTY(bool outline READ getOutline WRITE setOutline)

public:
	DiamondGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;

	QRect getDimensions();
	void setDimensions(QRect dimensions);
	/*! \brief Returns the width of this graphic in pixels.*/
	int getWidth(){return getDimensions().width();};
	/*! \brief Returns the height of this graphic in pixels.*/
	int getHeight(){return getDimensions().height();};
	/*! \brief Sets the width of this graphic in pixels.*/
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	/*! \brief Sets the height of this graphic in pixels.*/
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	/*! \brief Returns whether this graphic is set to display only its outline.*/
	bool getOutline(){return propertyContainer_->getPropertyValue("Outline").toBool();};
	/*! \brief Sets whether this graphic should display only its outline.*/
	void setOutline(bool outline) { propertyContainer_->setPropertyValue("Outline", outline);};
	QPoint getPositionOffset();
	void setAngle(double angle){ propertyContainer_->setPropertyValue("Angle", angle); };
	/*! \brief Returns whether this graphic is set to display only its outline.*/
	double getAngle(){ return propertyContainer_->getPropertyValue("Angle").toDouble(); };


	virtual QString friendlyTypeName(){return "Diamond";};
	virtual QString getUITemplate(){return "DiamondElement";};
public slots:
	/*! \brief Set the dimensions (width, height) of this graphic.*/
	void setDimensions(int w, int h){setWidth(w);setHeight(h);};
	/*! \brief Set the rotation angle used to rotate this graphic.*/
	void setRotationAngle(double a){ setAngle(a); };

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QPoint posOffset_;
private slots:
	
};


}; //namespace Picto

#endif
