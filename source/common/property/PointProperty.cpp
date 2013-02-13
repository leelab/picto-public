#include <QScriptValue>
#include <QObject>

#include "PointProperty.h"
#include "../memleakdetect.h"

namespace Picto {

PointProperty::PointProperty(QString name, QVariant value):
Property(QVariant::Point,name,value)
{
}

QString PointProperty::variantToString(QVariant value) const
{
	QPoint point = value.toPoint();
	return QString("%1,%2").arg(point.x()).arg(point.y());
}
QVariant PointProperty::stringToVariant(QString string, QString& error) const
{
	error = "";
	QStringList xy = string.split(",");
	if(xy.size() != 2)
	{
		error = "Invalid data entered for point property";
		return QPoint(0,0);
	}
	QPoint newVal(xy[0].toInt(),xy[1].toInt());
	return QVariant(newVal);
}

QVariant PointProperty::attributeMapToVariantValue(QMap<QString,QVariant> attrMap, QString& error) const
{
	QPoint point;
	if(!attrMap.contains("x") || !attrMap.contains("y"))
		error = "Point property defined with attribute values must contain both 'x' and 'y' attributes";
	point.setX(attrMap.value("x").toInt());
	point.setY(attrMap.value("y").toInt());
	return QVariant(point);
}
}; //namespace Picto
