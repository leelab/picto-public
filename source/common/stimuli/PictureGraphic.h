#ifndef _PICTUREGRAPHIC_H_
#define _PICTUREGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

class PICTOLIB_CLASS PictureGraphic : public VisualElement
{
	Q_OBJECT

public:
	PictureGraphic(QPoint position, QString imageFile);

	void draw();

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
