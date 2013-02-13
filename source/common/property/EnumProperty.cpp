#include <QScriptValue>
#include <QObject>

#include "EnumProperty.h"
#include "../memleakdetect.h"

namespace Picto {

EnumProperty::EnumProperty(QString name, QVariant value):
Property(typeId(),name,value)
{}

QString EnumProperty::variantToString(QVariant value) const
{
	int val = value.toInt();
	QStringList enumNames = attributeValue("enumNames").toStringList();
	Q_ASSERT((val >=0) && (val < enumNames.size()));
	return enumNames[val];
}

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
