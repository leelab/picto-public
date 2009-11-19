#include "Parameter.h"

namespace Picto {

Parameter::Parameter(int _type, QString _name, QVariant _value) :
	type_(_type),
	name_(_name),
	value_(_value)
{
}

int Parameter::type()
{
	return type_;
}

QString Parameter::name()
{
	return name_;
}

QVariant Parameter::value()
{
	return value_;
}

void Parameter::setValue(QVariant _value)
{
	value_ = _value;
}

QMap<QString, QVariant> Parameter::getAttributes()
{
	return attributes_;
}

void Parameter::addAttribute(QString _attributeName, QVariant _attributeValue)
{
	attributes_[_attributeName] = _attributeValue;
}

}; //namespace Picto
