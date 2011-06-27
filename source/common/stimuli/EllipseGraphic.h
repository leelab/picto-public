#ifndef _ELLIPSEGRAPHIC_H_
#define _ELLIPSEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief An ellipse graphic.
 *
 *	An EllipseGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the ellipse (in screen coordinates)
 *	- Dimensions: The width and height of the ellipse
 *	- Color: The color
 */

class PICTOLIB_CLASS EllipseGraphic : public VisualElement
{
	Q_OBJECT

public:
	EllipseGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new EllipseGraphic());};
	QRect getDimensions(){ return propertyContainer_->getPropertyValue("Dimensions").toRect(); };
	void setDimensions(QRect dimensions){ propertyContainer_->setPropertyValue("Dimensions",dimensions);};

	static const QString type;

public slots:
	int getWidth() { return getDimensions().width(); };
	void setWidth(int width);
	int getHeight() { return getDimensions().height(); };
	void setHeight(int height);
	void setDimensions(int width,int height) { setDimensions(QRect(0,0,width,height)); };

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
