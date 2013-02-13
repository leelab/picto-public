#include <QScriptValue>
#include <QObject>

#include "ColorProperty.h"
#include "../memleakdetect.h"

namespace Picto {

ColorProperty::ColorProperty(QString name, QVariant value):
Property(QVariant::Color,name,value)
{
}
QString ColorProperty::variantToString(QVariant value) const
{
	QColor color = value.value<QColor>();
	return QString("%1,%2,%3,%4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
}
QVariant ColorProperty::stringToVariant(QString string, QString& error) const
{
	error = "";
	//Check if the color is saved in the form #?????? (this is what a color QVariant->QString does)
	//but differs from our user friendly generated string.
	if(string.startsWith("#") && (string.length() == 7))
	{
		return QVariant(string).value<QColor>();
	}
	//It must be stored in our standard user friendly way ??,??,??,??
	QStringList colors = string.split(",");
	if(colors.size() != 4)
	{
		error = "Invalid data entered for color property";
		return QColor(0,0,0,0);
	}
	QColor newVal(colors[0].toInt(),colors[1].toInt(),colors[2].toInt(),colors[3].toInt());
	return QVariant(newVal);
}

}; //namespace Picto
