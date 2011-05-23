#include <QScriptValue>
#include <QObject>

#include "EnumProperty.h"

namespace Picto {

EnumProperty::EnumProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager):
Property(variantProp,manager)
{}
QString EnumProperty::valueString()
{
	int val = value().toInt();
	QStringList enumNames = variantProp_->attributeValue("enumNames").toStringList();
	Q_ASSERT((val >=0) && (val < enumNames.size()));
	return enumNames[val];
}
bool EnumProperty::SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//If the value is a number or a string of a number, just set it.
	QString num1 = QString::number(_value.toInt());
	QString num2 = _value.toString();
	if(QString::number(_value.toInt()) == _value.toString())
	{
		int val = _value.toInt();
		if((val < 0) || (val >= variantProp_->attributeValue("enumNames").toStringList().size()) )
		{
			QString errorString("The value entered is not valid for this property.\nValid entries are:");
			foreach(QString option,variantProp_->attributeValue("enumNames").toStringList())
			{
				errorString.append(QString("\n%1").arg(option));
			}
			addError(name().toAscii(), errorString.toAscii(), xmlStreamReader);
			return false;
		}
		Property::setValue(_value);
		return true;
	}

	// A string is being used to set the enum property.
	QStringList enumNames = variantProp_->attributeValue("enumNames").toStringList();
	// If user sets the enum value to anything thats not in the enum list then the value will be set to -1.
	// Otherwise it will be set to the correct index.
	int index = enumNames.indexOf(_value.toString());
	if(index < 0)
	{
		QString errorString("The value entered is not valid for this property.\nValid entries are:");
		foreach(QString option,variantProp_->attributeValue("enumNames").toStringList())
		{
			errorString.append(QString("\n%1").arg(option));
		}
		addError(name().toAscii(), errorString.toAscii(), xmlStreamReader);
		return false;
	}
	Property::setValue(QVariant(index));
	return true;
}

}; //namespace Picto
