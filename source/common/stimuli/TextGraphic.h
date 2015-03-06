
#ifndef _TEXTGRAPHIC_H_
#define _TEXTGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Displays text in the Experiment Scene.
 *
 *	Obviously this wouldn't be used with "SubjectView" enabled for Experiments with non-human
 *	subjects, but it is very useful with "OperatorView" for debugging and providing info on the operator screen.
 *
 *	At the moment, a TextGraphic is limited to 12 point Times, but there is no reason we
 *	couldn't expand its capablilites at some point.
 *
 *	Beyond the Properties inherited from VisualElement, this class adds a Size Property that is used for defining
 *	the Size of the Window in which text appears.  It is important to remember that if the Size is too small, text
 *	isn't going to shrink to fit it, it will just run off the end and be invisible.  For this reason, it is important
 *	to always be sure that the TextGraphic has a large enough Size to handle any text that we expect to be written.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS TextGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the width of the graphic window for the text in pixels.*/
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	/*! \brief Sets/gets the height of the graphic window for the text in pixels.*/
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	/*! \brief Sets/gets the text that is displayed in the graphic.*/
	Q_PROPERTY(QString text READ getText WRITE setText)
public:
	TextGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor(), QString text=QString());

	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;
	/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
	 *	\note The returned value contains no position information.  Only the Width, Height
	 *	values should be used.
	*/
	QRect getDimensions(){ return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize()); };
	/*! \brief Set the dimensions (width, height) of this graphic.
	 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
	 */
	void setDimensions(QRect dimensions){ propertyContainer_->setPropertyValue("Size",dimensions.size());};
	/*! \brief Returns the width of the graphic window for the text in pixels.*/
	int getWidth(){return getDimensions().width();};
	/*! \brief Returns the height of the graphic window for the text in pixels.*/
	int getHeight(){return getDimensions().height();};
	/*! \brief Sets the width of the graphic window for the text in pixels.*/
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	/*! \brief Sets the height of the graphic window for the text in pixels.*/
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	/*! \brief Gets the text that is displayed in the graphic.*/
	QString getText() { return propertyContainer_->getPropertyValue("Text").toString(); };
	/*! \brief Sets the text that is displayed in the graphic.*/
	void setText(QString text) { propertyContainer_->setPropertyValue("Text",text); };

	virtual QString friendlyTypeName(){return "Text";};
	virtual QString getUITemplate(){return "TextElement";};

public slots:
	/*! \brief Set the dimensions (width, height) of the graphic window for the text in this graphic.*/
	void setDimensions(int w, int h){setWidth(w);setHeight(h);};

protected:
	virtual QString defaultTagName(){return "Text";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	
};


}; //namespace Picto

#endif
