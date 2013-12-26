#include <QScriptValue>
#include <QObject>

#include "ColorProperty.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new ColorProperty with the input name and color QVariant value
*/
ColorProperty::ColorProperty(QString name, QVariant value):
Property(QVariant::Color,name,value)
{
}
/*! \brief Implements Property::stringToVariant to convert the input QVariant color to a r,g,b,a string
 *	like "255,122,0,100".
*/
QString ColorProperty::variantToString(QVariant value) const
{
	QColor color = value.value<QColor>();
	return QString("%1,%2,%3,%4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
}
/*! \brief Implements Property::stringToVariant() to convert the input string to a variant color.
 *	\details The input string may be in the format returned from variantToString() or in the #rrggbb where 
 *	rr, gg and bb are hex values from 0 to FF.  This is supported since QVariant color -> QString conversion
 *	returns colors in that format.
*/
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
