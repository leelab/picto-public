#ifndef _CIRCLEGRAPHIC_H_
#define _CIRCLEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS CircleGraphic : public VisualElement
{
	Q_OBJECT

public:
	CircleGraphic(QPoint position, int radius, QColor color);

	void draw();

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
