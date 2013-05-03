#include <QScriptValue>
#include <QObject>

#include "SizeProperty.h"
#include "../memleakdetect.h"

namespace Picto {

SizeProperty::SizeProperty(QString name, QVariant value):
Property(QVariant::Size,name,value)
{
}
QString SizeProperty::variantToString(QVariant value) const
{
	QSize size = value.toSize();
	return QString("%1,%2").arg(size.width()).arg(size.height());
}
QVariant SizeProperty::stringToVariant(QString string, QString& error) const
{
	error = "";
	QStringList wh = string.split(",");
	if(wh.size() != 2)
	{
		error = "Invalid data entered for Size property";
		return QSize(0,0);
	}
	QSize newVal(wh[0].toInt(),wh[1].toInt());
	return QVariant(newVal);
}
}; //namespace Picto
