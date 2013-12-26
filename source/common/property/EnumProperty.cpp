#include <QScriptValue>
#include <QObject>

#include "EnumProperty.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new EnumProperty with the input name and QVariant(int) value, where the int is the index of the 
 *	enum value that is selected.
 */
EnumProperty::EnumProperty(QString name, QVariant value):
Property(typeId(),name,value)
{}

/*! \brief Implements Property::stringToVariant to convert the input QVariant integer value to the corresponding
 *	enum name as a QString.  
 *	\details For example, if the EnumProperty holds (Small,Medium,Big), variantToString(1) would
 *	return "Medium".
*/
QString EnumProperty::variantToString(QVariant value) const
{
	int val = value.toInt();
	QStringList enumNames = attributeValue("enumNames").toStringList();
	Q_ASSERT((val >=0) && (val < enumNames.size()));
	return enumNames[val];
}

/*! \brief Implements Property::stringToVariant() to convert the input enum value name string the corresponding integer that
 *	it represents.  
 *	\details For example, if the EnumProperty holds (Small,Medium,Big), stringToVariant("Big") would
 *	return QVariant(2).  If the input string is not one of the enum value names, QVariant(-1) will
 *	be returned and an error message will be set to the input error string reference.
*/
QVariant EnumProperty::stringToVariant(QString string, QString& error) const
{
	error = "";
	QStringList enumNames = attributeValue("enumNames").toStringList();
	// If userString is not in the enum list then the value will be set to -1.
	// Otherwise it will be set to the correct index.
	int index = enumNames.indexOf(string);
	if(index < 0)
	{
		//Maybe the value is the index of the enum.  If so, use the indexed value
		if(QString::number(string.toInt()) == string)
		{
			int val = string.toInt();
			if((val > 0) || (val < attributeValue("enumNames").toStringList().size()) )
			{
				return QVariant(val);
			}
		}
		error = "The string entered for this enum property was not one of its possible values";
		return -1;
	}
	return QVariant(index);
}

}; //namespace Picto
