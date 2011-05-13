#include <QScriptValue>
#include <QObject>

#include "PointProperty.h"

namespace Picto {

PointProperty::PointProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager):
Property(variantProp,manager)
{
	AddSerializationAttribute("x");
	AddSerializationAttribute("y");
}
void PointProperty::UpdateSerializationAttributesFromValue()
{
	QPoint point = value().toPoint();
	SetSerializationAttributeValue("x",point.x());
	SetSerializationAttributeValue("y",point.y());
}
void PointProperty::SetValueFromDeserializedData(QVariant _value)
{
	QPoint point;
	point.setX(GetSerializationAttributeValue("x").toInt());
	point.setY(GetSerializationAttributeValue("y").toInt());\
	setValue(point);
}

}; //namespace Picto
