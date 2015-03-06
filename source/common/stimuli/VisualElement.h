#ifndef _VISUALELEMENT_H_
#define _VISUALELEMENT_H_
#include <QImage>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <QTime>

#include "../common.h"

#include "OutputElement.h"
#include "../statemachine/scriptable.h"
#include "../compositor/CompositingSurface.h"
#include "../random/MersenneTwister.h"
#include "../property/PropertyContainer.h"

namespace Picto {

/*!	\brief A base class for all elements that represent graphics to be drawn onto the the display's Scene.
 *
 *	\details This base class handles all of the StimulusElements that are visual in nature.  Generally used
 *	Properties like position, color, layer, visible are handled here.  A pure virtual draw() function is included 
 *	that must be implemented by descendant classes to draw their particular graphic to a QImage.  While descendant
 *	classes handle drawing the QImage though, this class handles drawing that QImage to the CompositingSurface
 *	that will be rendered onto a VisualTarget.  
 *	
 *	This is probably a good place to discuss Picto's 800x600 constraints.  Picto was developed with the requirement
 *	that the attached display uses 800 x 600 resolution.  This was obviously not a great assumption and should be
 *	fixed at some point.  Ideally we should probably have displayed a fixed aspect ratio window but left the resolution 
 *	up to the particular Director implementation.  In that system we could have confined the display window to have
 *	a 1x1 aspect ration and then simply defined positions as doubles from 0 to 1.  It may also make sense in the future
 *	to maintain the 4x3 aspect ratio and then just scale all 800 x 600 position values automatically.  
 *
 *	Another positioning issue is what part of an underlying graphic is positioned at the position set in this VisualElement.
 *	In the case of a circle for example, if we place it at 400x300 we would expect the circle to be in the center of the screen,
 *	but in the case of text, we would expect the top or bottom left corner to be used for positioning.  The graphics system 
 *	needs some way to figure out where this "handle" is located. For this purpose, we use the getPositionOffset() function to 
 *	define the offset from the top left corner of the VisualElement image to the point that should be used as its handle.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct PICTOLIB_CLASS VisualElement : public OutputElement
{
	Q_OBJECT
	/*! \brief Sets/gets this VisualElement's layer. Higher layer VisualElements are drawn on top of lower layer VisualElements.*/
	Q_PROPERTY(int layer READ getLayer WRITE setLayer)
	/*! \brief Sets/gets this VisualElement's visible value. \sa operatorView, subjectView*/
	Q_PROPERTY(bool visible READ getVisible WRITE setVisible)
	/*! \brief Sets/gets whether this VisualElement is visible to the experiment operator when it is set as visible.*/
	Q_PROPERTY(bool operatorView READ getOperatorView WRITE setOperatorView)
	/*! \brief Sets/gets whether this VisualElement is visible to the experiment subject when it is set as visible.*/
	Q_PROPERTY(bool subjectView READ getSubjectView WRITE setSubjectView)
	/*! \brief Sets/gets the x component of this VisualElement's position.*/
	Q_PROPERTY(int x READ getX WRITE setX)
	/*! \brief Sets/gets the y component of this VisualElement's position.*/
	Q_PROPERTY(int y READ getY WRITE setY)
	/*! \brief Sets/gets the red component of this VisualElement's color.*/
	Q_PROPERTY(int red READ getRed WRITE setRed)
	/*! \brief Sets/gets the green component of this VisualElement's color.*/
	Q_PROPERTY(int green READ getGreen WRITE setGreen)
	/*! \brief Sets/gets the blue component of this VisualElement's color.*/
	Q_PROPERTY(int blue READ getBlue WRITE setBlue)
	/*! \brief Sets/gets the alpha component of this VisualElement's color.*/
	Q_PROPERTY(int alpha READ getAlpha WRITE setAlpha)
	/*! \brief Sets/gets the this VisualElement's color in the \#rrbbgg format (ex. \#FF2255).  
	 *	\note This script property is compatible with values read from/written to a ColorParameter.
	 */
	Q_PROPERTY(QVariant color READ getColor WRITE setColor)
public:
	VisualElement(QPoint position=QPoint(), QColor color=QColor());
	virtual ~VisualElement();

	void addCompositingSurface(QString surfaceType, QSharedPointer<CompositingSurface> compositingSurface);
	QSharedPointer<CompositingSurface> getCompositingSurface(QString surfaceType);

	/*! \brief Draws the image represented by this VisualElement onto the image_ object based on its current Property values.*/
	virtual void draw() = 0;
	virtual void updateAnimation(int frame, QTime elapsedTime);

	QPoint getPosition();
	/*! \brief This returns the offset from the top left corner of the VisualElement image to the point in the image which should be 
	 * used for positioning.  For example, in a circle whose center should be positioned at the point entered into setPosition(), this
	 * function would return QPoint(radius,radius).
	 */
	virtual QPoint getPositionOffset(){return QPoint(0,0);};
	QRect getBoundingRect();
	void setPosition(QPoint position);

	QColor getColor();
	void setColor(QColor color);
	void setColor(QVariant color);

	/*! \brief Sets the layer of this VisualElement.  Higher layer VisualElements are drawn on top of lower layer VisualElements.*/
	void setLayer(int layer) { propertyContainer_->setPropertyValue("Layer",layer); };
	/*! \brief Returns the layer of this VisualElement.  Higher layer VisualElements are drawn on top of lower layer VisualElements.*/
	int getLayer() { return propertyContainer_->getPropertyValue("Layer").toInt(); }
	/*! \brief Sets whether the VisualElement is visible.
	 *	\note Even if the VisualElement is visible, it still must be visible for the subject and/or operator
	 *	in order for them to be able to see it.  
	 *	\sa ssetOperatorView(), setSubjectView()
	 */
	virtual void setVisible(bool visible){propertyContainer_->setPropertyValue("Visible",visible);};
	/*! \brief Gets whether the VisualElement is visible.
	 *	\note Even if the VisualElement is visible, it still must be visible for the subject and/or operator
	 *	in order for them to be able to see it.  
	 *	\sa getVisibleByUser(), getOperatorView(), getSubjectView()
	 */
	virtual bool getVisible(){return propertyContainer_->getPropertyValue("Visible").toBool();};
	bool getVisibleByUser(bool subject);
	/*! \brief Sets whether the VisualElement will be visible by the experiment operator if it is set visible overall.*/
	virtual void setOperatorView(bool visible){propertyContainer_->setPropertyValue("OperatorView",visible);};
	/*! \brief Gets whether the VisualElement will be visible by the experiment operator if it is set visible overall.*/
	virtual bool getOperatorView(){return propertyContainer_->getPropertyValue("OperatorView").toBool();};
	/*! \brief Sets whether the VisualElement will be visible by the experiment subject if it is set visible overall.*/
	virtual void setSubjectView(bool visible){propertyContainer_->setPropertyValue("SubjectView",visible);};
	/*! \brief Gets whether the VisualElement will be visible by the experiment subject if it is set visible overall.*/
	virtual bool getSubjectView(){return propertyContainer_->getPropertyValue("SubjectView").toBool();};
	/*! \brief Gets the x component of this VisualElement's position (0-800).*/
	int getX(){return getPosition().x();};
	/*! \brief Sets the x component of this VisualElement's position (0-800).*/
	void setX(int x){return setPosition(QPoint(x,getPosition().y()));};
	/*! \brief Gets the y component of this VisualElement's position (0-600).*/
	int getY(){return getPosition().y();};
	/*! \brief Sets the y component of this VisualElement's position (0-600).*/
	void setY(int y){return setPosition(QPoint(getPosition().x(),y));};
	virtual QString getUITemplate(){return "VisualElement";};
	virtual QString friendlyTypeName(){return "Visual Element";};
	/*! \brief Gets the red component of this VisualElement's color.*/
	int getRed() { return getColor().red(); };
	/*! \brief Gets the green component of this VisualElement's color.*/
	int getGreen() { return getColor().green(); };
	/*! \brief Gets the blue component of this VisualElement's color.*/
	int getBlue() { return getColor().blue(); };
	/*! \brief Gets the alpha component of this VisualElement's color.*/
	int getAlpha() { return getColor().alpha(); };
	/*! \brief Sets the red component of this VisualElement's color.*/
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	/*! \brief Sets the green component of this VisualElement's color.*/
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	/*! \brief Sets the blue component of this VisualElement's color.*/
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	/*! \brief Sets the alpha component of this VisualElement's color.*/
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

	void setScalable(bool scalable);	
	/*! \brief Gets this Visual Element scalable/non-scalable.  Visual Elements are scalable by default.
	 *	\details The only reason a visual element would be non-scalable is if it is a kind of UI only element that should not
	 *	scale down with the rest of the scene.  This is used for example with the red cursor that is visible
	 *	only by the operator.  If the operator zooms out, he doesn't want the cursor graphic
	 *	to get smaller.  It should stay the same size.  For that reason, the cursor graphic is set as non-scalable.
	 */
	bool scalable(){return scalable_;};

	virtual void upgradeVersion(QString deserializedVersion);

public slots:
	/*! \brief Sets the color of this VisualElement.
	 *	\details Inputs are from 0-255.
	 */
	void setColor(int r, int g, int b, int a=255){setColor(QColor(r,g,b,a));};
	/*! \brief Sets the position of this VisualElement in the Scene.
	 *	\details Inputs are x: 0-800, y: 0-600
	 */
	void setPos(int x, int y){setPosition(QPoint(x,y));};

protected:

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QImage image_;	//!< The image object underlying this VisualElement.  It contains the image that will actually be rendered on screen.
	bool shouldUpdateCompositingSurfaces_;	//!< Set this to true when the image has changed to let the VisualElement::updateAnimation() funciton know that it should call updateCompositingSurfaces(). 
	QMap<QString, QSharedPointer<CompositingSurface> > compositingSurfaces_;	//!< A lookup table containing all the CompositingSurface objects on which the image_ will be drawn.  Lookup is by compositing surface type name.
	
	void updateCompositingSurfaces();

	//MTRand random;

	//QMap<QString, QVariant> initialProperties_;
	//bool visible_;
private:
	bool scalable_;
private slots:
	virtual void slotPropertyValueChanged(Property* prop,QVariant);
};

}; //namespace Picto

#endif
