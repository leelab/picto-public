#ifndef _ELLIPSEGRAPHIC_H_
#define _ELLIPSEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS EllipseGraphic : public VisualElement
{
	Q_OBJECT

public:
	EllipseGraphic(QPoint position, QRect dimensions, QColor color);

	void draw();

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
