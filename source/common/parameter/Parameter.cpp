#include "Parameter.h"

namespace Picto {

Parameter::Parameter(int type) :
	bOperatorUI_(false),
	type_("null"),
	order_(0)
{
	AddDefinableProperty(type,"InitialValue",QVariant());
}

QString Parameter::type()
{
	return type_;
}

void Parameter::reset()
{
	currValue_ = propertyContainer_->getPropertyValue("InitialValue");
}

void Parameter::setInitialValue(QVariant val)
{
	propertyContainer_->setPropertyValue("InitialValue",verifyValue(val));
	//PUT IN VERIFY VALUE BUT WITH INPUT VALUE SO THAT THIS CAN BE VERIFIED TO!!!
}

void Parameter::setValue(QVariant value)
{
	currValue_ = verifyValue(value);
}

QVariant Parameter::getValue()
{
	return currValue_;
}

void Parameter::postSerialize()
{
	Scriptable::postSerialize();
	setInitialValue(propertyContainer_->getPropertyValue("InitialValue"));
	reset();
}


}; //namespace Picto
