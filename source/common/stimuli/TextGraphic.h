
#ifndef _TEXTGRAPHIC_H_
#define _TEXTGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Generates text stimuli.
 *
 *	Obviously this wouldn't be used in experiments with non-human
 *	subjects, but it can be very useful for debugging, providing 
 *	info on the operator screen, or running experiments with human
 *	subjects.
 *
 *	At the moment, a TextGraphic is limited to 12 point Times, but there is no reason we
 *	couldn't expand its capablilites.
 *
 *	A TextGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the grid (in screen coordinates)
 *	- Dimensions: The width and height of the text element
 *	- Color: The color of the text
 *	- Text: The text to be displayed
 */
class PICTOLIB_CLASS TextGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	Q_PROPERTY(QString text READ getText WRITE setText)
public:
	TextGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor(), QString text=QString());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	QRect getDimensions(){ return propertyContainer_->getPropertyValue("Dimensions").toRect(); };
	void setDimensions(QRect dimensions){ propertyContainer_->setPropertyValue("Dimensions",dimensions);};

	int getWidth(){return getDimensions().width();};
	int getHeight(){return getDimensions().height();};
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	QString getText() { return propertyContainer_->getPropertyValue("Text").toString(); };
	void setText(QString text) { propertyContainer_->setPropertyValue("Text",text); };

	static const QString type;

public slots:
	void setDimensions(int w, int h){setWidth(w);setHeight(h);};

protected:
	virtual QString defaultTagName(){return "Text";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	//void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
