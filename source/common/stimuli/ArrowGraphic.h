#ifndef _ARROWGRAPHIC_H_
#define _ARROWGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS ArrowGraphic : public VisualElement
{
	Q_OBJECT

public:
	ArrowGraphic(QPoint position, QPoint start, QPoint end, int size, QColor color);

	void draw();

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
