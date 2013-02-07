#include <QScriptValue>
#include <QObject>

#include "PointProperty.h"
#include "../memleakdetect.h"

namespace Picto {

PointProperty::PointProperty(QString name, QVariant value):
Property(QVariant::Point,name,value)
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
bool PointProperty::SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QPoint point;
	point.setX(GetSerializationAttributeValue("x").toInt());
	point.setY(GetSerializationAttributeValue("y").toInt());
	Property::setValue(point);
	return true;
}

QString PointProperty::toUserString()
{
	QPoint point = value().toPoint();
	return QString("%1,%2").arg(point.x()).arg(point.y());
}
void PointProperty::fromUserString(QString userString)
{
	QStringList xy = userString.split(",");
	Q_ASSERT(xy.size() == 2);
	QPoint newVal(xy[0].toInt(),xy[1].toInt());
	setValue(newVal);
}
}; //namespace Picto
