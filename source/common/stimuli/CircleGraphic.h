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
	CircleGraphic(QPoint position=QPoint(), int radius=0, QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();

	static const QString name;

private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
