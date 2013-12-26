#include <QScriptValue>
#include <QObject>

#include "SizeProperty.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new PointProperty with the input name and QVariant(QSize) value
*/
SizeProperty::SizeProperty(QString name, QVariant value):
Property(QVariant::Size,name,value)
{
}

/*! \brief Implements Property::stringToVariant to convert the input QVariant(QSize) to a "w,h" string
 *	like "50,100".
*/
QString SizeProperty::variantToString(QVariant value) const
{
	QSize size = value.toSize();
	return QString("%1,%2").arg(size.width()).arg(size.height());
}

/*! \brief Implements Property::stringToVariant() to convert the input string to a QVariant(QSize).
 *	\details The input string must be in the format returned from variantToString().  If the input
 *	string is not in the correct format, an error message is set to the error QString reference and a
 *	QVariant(QSize(0,0)) value is returned.
*/
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
