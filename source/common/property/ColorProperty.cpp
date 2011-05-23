#include <QScriptValue>
#include <QObject>

#include "ColorProperty.h"

namespace Picto {

ColorProperty::ColorProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager):
Property(variantProp,manager)
{
	AddSerializationAttribute("R");
	AddSerializationAttribute("G");
	AddSerializationAttribute("B");
	AddSerializationAttribute("A");
}
void ColorProperty::UpdateSerializationAttributesFromValue()
{
	QColor color = value().value<QColor>();
	SetSerializationAttributeValue("R",color.red() );
	SetSerializationAttributeValue("G",color.green());
	SetSerializationAttributeValue("B",color.blue());
	SetSerializationAttributeValue("A",color.alpha());
}
bool ColorProperty::SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QColor color;
	color.setRed(GetSerializationAttributeValue("R").toInt());
	color.setGreen(GetSerializationAttributeValue("G").toInt());
	color.setBlue(GetSerializationAttributeValue("B").toInt());
	color.setAlpha(GetSerializationAttributeValue("A").toInt());
	setValue(color);
	return true;
}

}; //namespace Picto
