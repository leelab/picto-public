#include <QScriptValue>

#include "Property.h"

namespace Picto {

Property::Property(int _type, QString _name, QVariant _value) :
	type_(_type),
	name_(_name),
	value_(_value)
{
}

int Property::type()
{
	return type_;
}

QString Property::name()
{
	return name_;
}

QVariant Property::value()
{
	return value_;
}

void Property::setValue(QVariant _value)
{
	value_ = _value;
}

QMap<QString, QVariant> Property::getAttributes()
{
	return attributes_;
}

void Property::addAttribute(QString _attributeName, QVariant _attributeValue)
{
	attributes_[_attributeName] = _attributeValue;
}


}; //namespace Picto
