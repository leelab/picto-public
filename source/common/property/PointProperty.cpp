#include <QScriptValue>
#include <QObject>

#include "PointProperty.h"
#include "../memleakdetect.h"

namespace Picto {
/*! \brief Constructs a new PointProperty with the input name and QVariant(QPoint) value
*/
PointProperty::PointProperty(QString name, QVariant value):
Property(QVariant::Point,name,value)
{
}

/*! \brief Implements Property::stringToVariant to convert the input QVariant(QPoint) to a "x,y" string
 *	like "400,300".
*/
QString PointProperty::variantToString(QVariant value) const
{
	QPoint point = value.toPoint();
	return QString("%1,%2").arg(point.x()).arg(point.y());
}

/*! \brief Implements Property::stringToVariant() to convert the input string to a QVariant(QPoint).
 *	\details The input string must be in the format returned from variantToString().  If the input
 *	string is not in the correct format, an error message is set to the error QString reference and a
 *	QVariant(QPoint(0,0)) value is returned.
*/
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

/*! \brief Implements Property::attributeMapToVariantValue() to upgrade the PointProperty serialization syntax from older
 *	versions where the x,y values were stored in an XML tag's attributes.
 *	\details In older versions of the Design syntax, the Xml tag contained \code x='' y='' \endcode values to describe the
 *	QPoint value.  Now, we put the value between the start and end tags of the Property.  This function takes in the table
 *	of Xml attributes and sets a QVariant value accordingly.  It is then set as this Property's value by the Property class
 *	code so that on the next serialization, the Design syntax will be upgraded automatically.
 */
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
