#ifndef _RANDOMLYFILLEDGRIDGRAPHIC_H_
#define _RANDOMLYFILLEDGRIDGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A graphic that contains a grid filled with two separate colors of squares.  The positions of the different square colors is random and rerandomized
 *	at a preset frequency.  The relative number of each color of squares is maintained over randomization.
 *
 *	\details Randomly filled grids are used in many neuro physiology experiments.  These are rectangular grids in which each space in the grid is filled with one 
 *	of two colors, at random, according to a set distribution.  The two colors, the number of squares in the grid and how it is split up into rows and columns 
 *	can all be set.  The designer can also set the rate at which the grid colors are re-randomized, and of course things like the grid size and position.
 *	
 *	\details This is one of those graphics that was created toward the beginning of the Picto project but wasn't used in any actual experiments.  For this reason
 *	it is missing some important things like Script Properties.  It should really be brought up to date.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */


class PICTOLIB_CLASS RandomlyFilledGridGraphic : public VisualElement
{
	Q_OBJECT

public:
	RandomlyFilledGridGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), 
								QColor color=QColor(), QColor color2=QColor(), 
								int numHorizSquares=1, int numVertSquares=1,int numColor2=1,
								int updateFrameRate=0);

	void draw();
	void updateAnimation(int frame, QTime elapsedTime);
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	static const QString type;

	/*! \brief Sets the color of the second set of squares that are displayed in this graphic.*/
	void setColor2(QColor color) { propertyContainer_->setPropertyValue("Color2",color); draw(); };
	/*! \brief Gets the color of the second set of squares that are displayed in this graphic.*/
	QColor getColor2() { return propertyContainer_->getPropertyValue("Color2").value<QColor>(); };
	/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
	 *	\note The returned value contains no position information.  Only the Width, Height
	 *	values should be used.
	*/
	QRect getDimensions() { return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize()); };
	/*! \brief Set the dimensions (width, height) of this graphic.
	 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
	 */
	void setDimensions(QRect dimensions) { propertyContainer_->setPropertyValue("Size",dimensions.size()); };
	QPoint getPositionOffset();

	virtual QString friendlyTypeName(){return "Random Fill Grid";};
	virtual QString getUITemplate(){return "RandomGridElement";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void buildColorList();

	QVector<unsigned char> colorList_;
	QPoint posOffset_;
	int prevColor2Squares_;
	int prevHorizSquares_;
	int prevVertSquares_;

private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
