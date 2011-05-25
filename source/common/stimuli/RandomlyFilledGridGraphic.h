#ifndef _RANDOMLYFILLEDGRIDGRAPHIC_H_
#define _RANDOMLYFILLEDGRIDGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A randomly filled grid graphic.
 *
 *	Randomly filled grids are used in many neuro physiology experiments.  These are rectangular grids
 *	in which each space in the grid is filled with one of two colors, at random, according to a given 
 *	distribution.
 *	
 *	A RandomlyFilledGridGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the grid (in screen coordinates)
 *	- Dimensions: The width and height of the grid
 *	- Color: The first color
 *	- Color2: The second color
 *	- Number of horizontal squares: As advertised
 *	- Number of vertical squares: As advertised
 *	- Number of squares with color1: This lets you set the ratio of colored grid spaces
 *	- Animated: Does the grid arrangement change?  Or is it static? (boolean)
 *	- Update frame rate: If the grid is animated, this is the frequency at which it updates.  Otherwise meaningless.
 */


class PICTOLIB_CLASS RandomlyFilledGridGraphic : public VisualElement
{
	Q_OBJECT

public:
	RandomlyFilledGridGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), 
								QColor color1=QColor(), QColor color2=QColor(), 
								int numHorizSquares=1, int numVertSquares=1,int numColor1=1, 
								bool animated=false, int updateFrameRate=0);

	void draw();
	void updateAnimation(int frame, QTime elapsedTime);
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new RandomlyFilledGridGraphic());};
	static const QString type;

	void setColor2(QColor color) { propertyContainer_->setPropertyValue("Color2",color); draw(); };
	QColor getColor2() { return propertyContainer_->getPropertyValue("Color2").value<QColor>(); };
	QRect getDimensions() { return propertyContainer_->getPropertyValue("Dimensions").toRect(); };
	void setDimensions(QRect dimensions) { propertyContainer_->setPropertyValue("Dimensions", dimensions); };


//These are for script binding
public slots:
	int getRed2() { return getColor2().red(); };
	int getBlue2() { return getColor2().blue(); };
	int getGreen2() { return getColor2().green(); };
	int getAlpha2() { return getColor2().alpha(); };
	void setColor2(int r, int g, int b, int a=255) { setColor2(QColor(r,g,b,a)); };

	int getWidth() { return getDimensions().width(); };
	void setWidth(int width);
	int getHeight() { return getDimensions().height(); };
	void setHeight(int height);

	int getNumHorizSquares() { return propertyContainer_->getPropertyValue("Number of horizontal squares").toInt(); };
	void setNumHorizSquares(int squares) { propertyContainer_->setPropertyValue("Number of horizontal squares",squares); buildColorList(); draw();};

	int getNumVertSquares() { return propertyContainer_->getPropertyValue("Number of vertical squares").toInt(); };
	void setNumVertSquares(int squares) { propertyContainer_->setPropertyValue("Number of vertical squares",squares); buildColorList(); draw();};

	int getNumColor1Squares() { return propertyContainer_->getPropertyValue("Number of squares with color1").toInt(); };
	void setNumColor1Squares(int squares) { propertyContainer_->setPropertyValue("Number of squares with color1",squares); buildColorList(); draw();};

	int getUpdateRate() { return propertyContainer_->getPropertyValue("Update frame rate").toInt(); };
	void setUpdateRate(int rate) { propertyContainer_->setPropertyValue("Update frame rate",rate);};

	void setAnimated(bool animated) { propertyContainer_->setPropertyValue("Animated",animated);};
	bool isAnimated() { return propertyContainer_->getPropertyValue("Animated").toBool(); };
	

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);


private:
	void buildColorList();

	QList<unsigned char> colorList_;
};


}; //namespace Picto

#endif
