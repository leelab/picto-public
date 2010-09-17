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

public:
	CircleGraphic(QPoint position=QPoint(), int radius=0, QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();

	static const QString type;

public slots:
	int getRadius() { return propertyContainer_.getPropertyValue("Radius").toInt(); };
	void setRadius(int radius) { propertyContainer_.setPropertyValue("Radius",radius);};

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
