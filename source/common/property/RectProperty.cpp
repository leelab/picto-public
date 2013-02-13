#include <QScriptValue>
#include <QObject>

#include "RectProperty.h"
#include "../memleakdetect.h"

namespace Picto {

RectProperty::RectProperty(QString name, QVariant value):
Property(QVariant::Rect,name,value)
{
}
QString RectProperty::variantToString(QVariant value) const
{
	QRect rect = value.toRect();
	return QString("%1,%2,%3,%4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
}
QVariant RectProperty::stringToVariant(QString string, QString& error) const
{
	error = "";
	QStringList xy = string.split(",");
	if(xy.size() != 4)
	{
		error = "Invalid data entered for Rectangle property";
		return QRect(0,0,0,0);
	}
	QRect newVal(xy[0].toInt(),xy[1].toInt(),xy[2].toInt(),xy[3].toInt());
	return QVariant(newVal);
}

QVariant RectProperty::attributeMapToVariantValue(QMap<QString,QVariant> attrMap, QString& error) const
{
	QRect rect;
	if(!attrMap.contains("x") || !attrMap.contains("y") || !attrMap.contains("width") || !attrMap.contains("height"))
		error = "Rectangle property defined with attribute values must contain'x','y','width' and 'height' attributes";
	rect.setX(attrMap.value("x").toInt());
	rect.setY(attrMap.value("y").toInt());
	rect.setWidth(attrMap.value("width").toInt());
	rect.setHeight(attrMap.value("height").toInt());
	return QVariant(rect);
}
}; //namespace Picto
