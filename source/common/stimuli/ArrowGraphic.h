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
	ArrowGraphic(QPoint position=QPoint(), QPoint start=QPoint(), QPoint end=QPoint(),
				int size=0, QColor color=QColor());

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
