#ifndef _CIRCLEGRAPHIC_H_
#define _CIRCLEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A circle graphic.
 *
 *	A CircleGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the circle (in screen coordinates)
 *	- Radius: Duh.
 *	- Color: The color of the circle
 */

class PICTOLIB_CLASS CircleGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int radius READ getRadius WRITE setRadius)
	Q_PROPERTY(bool outline READ getOutline WRITE setOutline)
public:
	CircleGraphic(QPoint position=QPoint(), int radius=0, QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	static const QString type;
	QPoint getPositionOffset();
	int getRadius(){return propertyContainer_->getPropertyValue("Radius").toInt();};
	void setRadius(int radius) { propertyContainer_->setPropertyValue("Radius", radius);};
	bool getOutline(){return propertyContainer_->getPropertyValue("Outline").toBool();};
	void setOutline(bool outline) { propertyContainer_->setPropertyValue("Outline", outline);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QPoint posOffset_;
private slots:
	
};


}; //namespace Picto

#endif
