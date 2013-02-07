#include <QScriptValue>
#include <QObject>

#include "RectProperty.h"
#include "../memleakdetect.h"

namespace Picto {

RectProperty::RectProperty(QString name, QVariant value):
Property(QVariant::Rect,name,value)
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
bool RectProperty::SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QRect rect;
	rect.setX(GetSerializationAttributeValue("x").toInt());
	rect.setY(GetSerializationAttributeValue("y").toInt());
	rect.setWidth(GetSerializationAttributeValue("width").toInt());
	rect.setHeight(GetSerializationAttributeValue("height").toInt());
	Property::setValue(rect);
	return true;
}
QString RectProperty::toUserString()
{
	QRect rect = value().toRect();
	return QString("%1,%2,%3,%4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
}
void RectProperty::fromUserString(QString userString)
{
	QStringList xy = userString.split(",");
	Q_ASSERT(xy.size() == 4);
	QRect newVal(xy[0].toInt(),xy[1].toInt(),xy[2].toInt(),xy[3].toInt());
	setValue(newVal);
}
}; //namespace Picto
