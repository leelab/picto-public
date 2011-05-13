#include <QScriptValue>
#include <QObject>

#include "RectProperty.h"

namespace Picto {

RectProperty::RectProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager):
Property(variantProp,manager)
{
	AddSerializationAttribute("x");
	AddSerializationAttribute("y");
	AddSerializationAttribute("width");
	AddSerializationAttribute("height");
}
void RectProperty::UpdateSerializationAttributesFromValue()
{
	QRect rect = value().toRect();
	SetSerializationAttributeValue("x",rect.x());
	SetSerializationAttributeValue("y",rect.y());
	SetSerializationAttributeValue("width",rect.width());
	SetSerializationAttributeValue("height",rect.height());
}
void RectProperty::SetValueFromDeserializedData(QVariant _value)
{
	QRect rect;
	rect.setX(GetSerializationAttributeValue("x").toInt());
	rect.setY(GetSerializationAttributeValue("y").toInt());
	rect.setWidth(GetSerializationAttributeValue("width").toInt());
	rect.setHeight(GetSerializationAttributeValue("height").toInt());
	setValue(rect);
}

}; //namespace Picto
