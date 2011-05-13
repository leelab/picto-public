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
void EnumProperty::setValue(QVariant _value)
{
	//If the value is a number or a string of a number, just set it.
	QString num1 = QString::number(_value.toInt());
	QString num2 = _value.toString();
	if(QString::number(_value.toInt()) == _value.toString())
	{
		Property::setValue(_value);
		return;
	}

	// A string is being used to set the enum property.
	QStringList enumNames = variantProp_->attributeValue("enumNames").toStringList();
	// If user sets the enum value to anything thats not in the enum list then the value will be set to -1.
	// Otherwise it will be set to the correct index.
	int index = enumNames.indexOf(_value.toString());
	Property::setValue(QVariant(index));
}

}; //namespace Picto
