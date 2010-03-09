#ifndef _LINEGRAPHIC_H_
#define _LINEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

#include <QVector>
#include <QPoint>

namespace Picto {

class PICTOLIB_CLASS LineGraphic : public VisualElement
{
	Q_OBJECT

public:
	LineGraphic(QPoint position=QPoint(), QVector<QPoint> = QVector<QPoint>(2,QPoint()), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();

	static const QString type;

private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	void slotPropertyValueChanged(QString propertyName, QVariant propertyValue);
};


}; //namespace Picto

#endif
