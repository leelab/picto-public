#include <QScriptValue>
#include <QObject>

#include "ColorProperty.h"
#include "../memleakdetect.h"

namespace Picto {

ColorProperty::ColorProperty(QtVariantProperty* variantProp, QtVariantPropertyManager* manager):
Property(variantProp,manager)
{
	AddSerializationAttribute("R");
	AddSerializationAttribute("G");
	AddSerializationAttribute("B");
	AddSerializationAttribute("A");
}
QString ColorProperty::toUserString()
{
	QColor color = value().value<QColor>();
	return QString("%1,%2,%3,%4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
}
void ColorProperty::fromUserString(QString userString)
{
	QStringList colors = userString.split(",");
	Q_ASSERT(colors.size() == 4);
	QColor newVal(colors[0].toInt(),colors[1].toInt(),colors[2].toInt(),colors[3].toInt());
	setValue(newVal);
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
