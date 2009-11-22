#ifndef _BOXGRAPHIC_H_
#define _BOXGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS BoxGraphic : public VisualElement
{
	Q_OBJECT

public:
	BoxGraphic(QPoint position, QRect dimensions, QColor color);

	void draw();

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
